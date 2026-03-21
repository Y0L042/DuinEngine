// Component definitions for Daslang evaluation test

[export]
struct Vec2 {
    x : float;
    y : float;
}

[export]
struct GravityComponent {
    x : float = 0.0;
    y : float = -9.81;
    z : float = 0.0;
}

[export]
struct Health {
    current : float = 100.0;
    max_ : float = 100.0;
}

[export]
struct SpeedTag {
    dummy : int = 0;  // daslang requires at least one field
}