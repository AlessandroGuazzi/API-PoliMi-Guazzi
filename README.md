# Algoritmi e Principi dell'Informatica's project - A.Y. 2022-23

## 📖 Overview

This project was developed as the **final assignment** for the course *Algoritmi e Principi dell’Informatica* (Algorithms and Principles of Computer Science) during my Bachelor’s degree in Computer Science at Politecnico di Milano,
and focuses on the practical application of algorithms and data structures to solve an efficiency-critical problem.

---

## 🎯 Objectives

The main goal of the project is to design and implement an efficient algorithm to solve the following problem:

* The highway is represented as a sequence of **service stations**, each located at a unique distance from the starting point.
* Each station manages a **fleet of electric vehicles**, each with a specific **range (autonomy in km)**.
* When renting a car from a station, it is possible to reach all other stations within its autonomy.
* A journey consists of a sequence of stations, starting at one and ending at another, always moving **forward along the highway**. At each stop, a new vehicle must be rented.
* The objective is to **find the path with the minimum number of stops** between two given stations.
* In case of multiple optimal solutions, the algorithm must prefer the path that minimizes the lexicographic order of station distances.

This project therefore combines **graph representation, shortest path algorithms, and lexicographic tie-breaking rules**, with a strong emphasis on **time and memory efficiency**.

---

## ⚙️ Implementation

* **Language**: C
* **Libraries**: only the C standard library (no external dependencies)
* **I/O**:

  * Input is received from **stdin**
  * Output is written to **stdout**

The solution was designed to be efficient both in terms of **runtime** and **memory usage**, as evaluated through automated tests.

---

## 📥 Input Format

The program processes a sequence of commands, one per line, with the following operations:

* `aggiungi-stazione distanza numero-auto autonomia_1 ... autonomia_n`
* `demolisci-stazione distanza`
* `aggiungi-auto distanza-stazione autonomia`
* `rottama-auto distanza-stazione autonomia`
* `pianifica-percorso partenza arrivo`

For each command, the program outputs a corresponding result (e.g., `aggiunta`, `non aggiunta`, `demolita`, `nessun percorso`, or the list of station distances representing the journey).

---

## 🔎 How It Works

The program implements a **management and path-planning system for highway service stations**.
Stations are stored in a **hash map**, each containing a fleet of cars with different driving ranges. The goal is to efficiently compute the **optimal route** between two stations, minimizing the number of stops (i.e., car changes).

The core logic is based on a **variant of Dijkstra’s algorithm**, adapted to this structure:

* **Data structures**:

  * Each station is identified by its distance from the highway start and stores its available cars.
  * Each car has a range, and the maximum available range per station is tracked.
  * Stations are stored in a fixed-size hash map for fast lookup, with linked lists to handle collisions.

* **Commands**:
  The system processes input commands to add/remove stations, add/remove cars, or request a path calculation between two stations.

* **Graph construction**:
  When a route is requested, a graph is built dynamically. Each station is connected to those reachable within the maximum range of its cars, either moving forward or backward depending on the journey direction.

* **Path calculation**:
  A modified Dijkstra’s algorithm is applied to compute the shortest path in terms of stops.

  * If multiple optimal paths exist, the algorithm selects the lexicographically smallest one (preferring stations closer to the highway start).
  * If no valid path exists, the program outputs `"nessun percorso"`.

This approach allows efficient management of dynamic station data and ensures correctness under strict performance constraints on **time and memory**.

---

## 🛠️ How to Compile and Run

1. Compile the program with a C compiler (e.g., `gcc`):

   ```bash
   gcc -Wall -Werror -std=gnu11 -O2 -lm -o highway main.c
   ```

   * `-std=gnu11` C11 plus GNU-specific extensions
   * `-O2` enables optimization for efficiency
   * `-lm` links the math library (*libm*).
   * `-Wall -Wextra` treats all warnings as errors. If the compiler finds even one warning, the build will fail. This ensures clean, warning-free code.

2. Run the program by providing commands via **stdin**. For example:

   ```bash
   ./highway < input.txt > output.txt
   ```

   where:

   * `input.txt` contains a list of commands
   * `output.txt` will store the results produced by the program
   
   then, in order to compare the file with the results use:

   ```bash
   diff ./results.txt ./output.txt
   ```

---

## 🏆 Final Evaluation

The project was graded based on:

* **Correctness**: verified through extensive automated test cases
* **Efficiency**: runtime and memory usage were measured by the testing system
* **Adherence to specifications**: correct handling of all operations and edge cases

Grade: 30 / 30

---

