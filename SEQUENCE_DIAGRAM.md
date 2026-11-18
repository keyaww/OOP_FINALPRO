# 🎱 Billiards Architecture - Sequence Diagrams

This document illustrates the interactions between the 6 components:
1. `main`
2. `Game`
3. `Table`
4. `Ball`
5. `CueStick`
6. `Physics`

---

## 1. Initialization & Game Start

When the application boots up, `main` instantiates `Game`, which sets up the table, cue stick, and balls.

```mermaid
sequenceDiagram
    autonumber
    actor User
    participant Main as main()
    participant Game as Game
    participant Table as Table
    participant Cue as CueStick
    participant Ball as Ball

    User->>Main: Launch application
    Main->>Game: Game() constructor
    activate Game
    Game->>Game: sf::RenderWindow(1024x600)
    Game->>Table: Table() [Setup rails, felt & 6 pockets]
    Game->>Cue: CueStick() [Initialize shaft & aim defaults]
    Game->>Game: setupUI() [Play, Quit, Title buttons]
    Game->>Table: getPlayArea()
    Table-->>Game: Playable felt bounds
    Game->>Game: setupBalls()
    activate Game
    Game->>Ball: Ball(id=0, CueBall, startPos)
    loop 15 Object Balls
        Game->>Ball: Ball(id, Solid/Stripe, rackPos, color)
    end
    deactivate Game
    deactivate Game

    Main->>Game: run()
    activate Game
    Note over Game: State: GameState::Menu
    User->>Game: Clicks "Play" button
    Game->>Game: m_state = Playing
    Game->>Cue: setVisible(true)
```

---

## 2. Aiming, Power Charge & Shooting (User Input)

When all balls are at rest, the cue stick tracks the mouse. Holding and releasing the left mouse button shoots the cue ball.

```mermaid
sequenceDiagram
    autonumber
    actor User
    participant Game as Game
    participant Physics as Physics
    participant Cue as CueStick
    participant CueBall as Ball (Cue)

    Note over Game: In GameState::Playing loop
    Game->>Physics: allAtRest(balls)
    Physics-->>Game: true

    User->>Game: Moves mouse
    Game->>Cue: setVisible(true)
    Game->>Cue: update(window, cueBall)
    Note over Cue: Computes angle from cueBall to cursor

    User->>Game: MouseButtonPressed (Left Click)
    Note over Game: m_shooting = true

    loop Every frame mouse is held
        Game->>Game: m_shootPower += 400 * dt
        Game->>Cue: setPower(m_shootPower)
    end

    User->>Game: MouseButtonReleased (Left Click)
    Note over Game: m_shooting = false
    Game->>Cue: shoot(m_shootPower)
    Cue-->>Game: shotVelocity (Vector2f)
    Game->>CueBall: setVelocity(shotVelocity)
    Game->>Cue: setPower(0)
    Game->>Cue: setVisible(false)
```

---

## 3. Physics Simulation & Pocketing Loop

Every frame while balls are in motion, velocities are updated, friction is applied, collisions are resolved, and pockets are inspected.

```mermaid
sequenceDiagram
    autonumber
    participant Game as Game
    participant Ball as Ball (All)
    participant Physics as Physics
    participant Table as Table

    Note over Game: updatePlaying(dt)

    loop For each ball
        Game->>Ball: update(dt)
        activate Ball
        Ball->>Ball: setPosition(pos + vel * dt)
        Ball->>Ball: applyFriction(dt)
        deactivate Ball
    end

    Game->>Physics: resolveAllCollisions(balls)
    activate Physics
    loop For each pair (Ball A, Ball B)
        Physics->>Ball: Check distance & overlap
        alt Colliding
            Physics->>Ball: Adjust positions (prevent overlap)
            Physics->>Ball: Elastic 1D normal velocity swap
        end
    end
    deactivate Physics

    Game->>Table: getPlayArea()
    Table-->>Game: Cushion boundary dimensions (FloatRect)
    loop For each ball
        Game->>Physics: resolveWalls(ball, playArea)
        alt Bouncing against cushion
            Physics->>Ball: Invert vel.x or vel.y
        end
    end

    Game->>Physics: checkPockets(balls, table)
    activate Physics
    loop For each ball
        Physics->>Table: isInPocket(ball.getPosition(), hit)
        alt Inside pocket radius
            Physics->>Ball: setPocketed(true), setVelocity(0,0)
        end
    end
    Physics-->>Game: List of pocketed ball IDs
    deactivate Physics

    Note over Game: Frame Render Step
    Game->>Table: draw(window) [Render rails, felt & 6 pockets]
    Game->>Ball: draw(window) [Render active balls]

    alt Cue Ball (ID 0) Pocketed
        Game->>Game: respawnCueBall() (Back to head string)
    else Object Ball Pocketed
        Game->>Game: Increment m_solidsPocketed / m_stripesPocketed
        Game->>Game: checkVictory()
        alt All solids or all stripes pocketed
            Game->>Game: m_state = GameState::Victory
        end
    end
```

---

## 4. Victory Screen & Replay Loop

When the match concludes, the game transitions to the Victory state with a Continue button that restarts the cycle.

```mermaid
sequenceDiagram
    autonumber
    actor User
    participant Game as Game
    participant Cue as CueStick

    Note over Game: State: GameState::Victory
    Game->>Game: renderVictory() [Overlay, Winner Text, Continue]

    User->>Game: Clicks "Continue"
    Game->>Game: m_state = GameState::Menu
    Game->>Cue: setVisible(false)
    Game->>Game: setupBalls() [Resets rack & cue ball]
```
