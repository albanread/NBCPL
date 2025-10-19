#!/usr/bin/env python3
import time
from collections import defaultdict


class Timer:
    def __init__(self):
        self.metrics = defaultdict(lambda: {"total_time": 0.0, "call_count": 0})

    def start(self, name):
        self._start_time = time.perf_counter_ns()
        self._current_timer = name

    def end(self, name):
        if not hasattr(self, "_start_time") or self._current_timer != name:
            print(f"Warning: Timer '{name}' not started properly")
            return

        end_time = time.perf_counter_ns()
        elapsed = end_time - self._start_time

        self.metrics[name]["total_time"] += elapsed
        self.metrics[name]["call_count"] += 1
        self._start_time = None
        self._current_timer = None

    def display(self):
        print("\n--- Performance Metrics ---")
        if not self.metrics:
            print("  (No timing metrics collected)")
            return

        # Calculate max name length for formatting
        max_name_len = max(len(name) for name in self.metrics.keys())
        max_name_len = max(max_name_len, 8)  # Minimum width

        header = (
            f"{'Function':<{max_name_len}} {'Calls':>8} {'Total':>12} {'Average':>12}"
        )
        separator = "-" * len(header)

        print(header)
        print(separator)

        for name, data in sorted(self.metrics.items()):
            total_ns = data["total_time"]
            calls = data["call_count"]
            avg_ns = total_ns / calls if calls > 0 else 0

            # Format time with appropriate units (ns, µs, ms)
            def format_time(nanoseconds):
                if nanoseconds < 1000:
                    return f"{nanoseconds:.0f} ns"
                elif nanoseconds < 1000000:
                    return f"{nanoseconds / 1000:.3f} µs"
                elif nanoseconds < 1000000000:
                    return f"{nanoseconds / 1000000:.3f} ms"
                else:
                    return f"{nanoseconds / 1000000000:.3f} s"

            print(
                f"{name:<{max_name_len}} {calls:>8} {format_time(total_ns):>12} {format_time(avg_ns):>12}"
            )

    def get_total_ns(self, name):
        return int(self.metrics[name]["total_time"])

    def get_call_count(self, name):
        return self.metrics[name]["call_count"]

    def clear(self):
        self.metrics.clear()


# Global timer instance
timer = Timer()


def recursive_factorial(n):
    """Pure recursive factorial without internal timing"""
    if n <= 1:
        return 1
    else:
        return n * recursive_factorial(n - 1)


def iter_factorial(n):
    """Pure iterative factorial without internal timing"""
    result = 1
    for i in range(2, n + 1):
        result *= i
    return result


def main():
    print("Hello")

    # Test both factorial implementations 10 times each
    for _ in range(10):
        # Time the entire recursive operation
        timer.start("recursive_factorial")
        recursive_result = recursive_factorial(18)
        timer.end("recursive_factorial")

        # Time the entire iterative operation
        timer.start("iter_factorial")
        iterative_result = iter_factorial(18)
        timer.end("iter_factorial")

        print(
            f"Calculating recursive Factorial of 18 = {recursive_result} AND = {iterative_result}"
        )

    print("BYE")

    # Display timing metrics
    timer.display()

    # Example of querying specific metrics (like BCPL version)
    print(f"\nDetailed metrics:")
    print(f"Recursive calls: {timer.get_call_count('recursive_factorial')}")
    print(f"Iterative calls: {timer.get_call_count('iter_factorial')}")
    print(f"Recursive total time: {timer.get_total_ns('recursive_factorial')} ns")
    print(f"Iterative total time: {timer.get_total_ns('iter_factorial')} ns")


if __name__ == "__main__":
    main()
