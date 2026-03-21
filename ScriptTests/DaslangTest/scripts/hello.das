options gen2

require components

var depth = 0;

[export]
def test() {
    print("Hello from daslang! Konichiwa\n");
    // hello();
    // fibonacci(999, 987)
    // find_primes(100000)
}

def hello() {
    print("Hello from hello()!\n");
    var pos : Vec2 = Vec2(x=2.0f, y=2.0f);
    print("Vec2 \{{pos.x}, {pos.y}\}");
}

def fibonacci(a, b : int) {
    print("Fibonacci stress test starting with a={a}, b={b}\n")
    var fib_a = a
    var fib_b = b
    var iterations = 100
    
    for (i in range(0, iterations)) {
        print("F({i}) = {fib_a}\n")
        var temp = fib_a + fib_b
        fib_a = fib_b
        fib_b = temp
    }
    
    print("Fibonacci stress test completed\n")
}

def is_prime(n : int) : bool {
    if (n < 2) {
        return false
    }
    if (n == 2) {
        return true
    }
    if (n % 2 == 0) {
        return false
    }
    var i = 3
    while (i * i <= n) {
        if (n % i == 0) {
            return false
        }
        i += 2
    }
    return true
}

def find_primes(count : int) {
    print("Finding {count} prime numbers...\n")
    var primes_found = 0
    var candidate = 2
    
    while (primes_found < count) {
        if (is_prime(candidate)) {
            print("Prime {primes_found + 1}: {candidate}\n")
            primes_found += 1
        }
        candidate += 1
    }
    
    print("Prime search completed. Found {primes_found} primes.\n")
}