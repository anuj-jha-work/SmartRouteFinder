# 🧭 Smart Route Finder (Dijkstra Algorithm | Deployed)

## 📌 Overview

This project implements a Smart Route Finder that calculates the shortest path between two nodes using Dijkstra’s Algorithm.
It focuses on core Data Structures and Algorithms concepts and is deployed as a web-accessible service.

---

## 🌐 Live Demo

https://smart-route-finder-cpp.onrender.com/

---

## 🚀 Features

* Compute shortest path between two nodes
* Uses weighted graph representation
* Efficient path calculation using priority queue (min-heap)
* Deployed application accessible via browser

---

## 🛠️ Tech Stack

* Language: C++
* Concepts: Graphs, Dijkstra Algorithm, Priority Queue
* Deployment: Render

---

## ⚙️ System Flow

1. User provides input (source and destination nodes)
2. Backend (C++ program) processes the request
3. Graph is traversed using Dijkstra’s algorithm
4. Shortest path and distance are computed
5. Result is returned and displayed

---

## 🧠 Key Concepts Used

* **Graph Representation (Adjacency List)**
* **Dijkstra Algorithm (Greedy approach)**
* **Priority Queue (Min-Heap)** for efficient selection
* **Time Complexity:** O((V + E) log V)

---

## ▶️ How to Run Locally

1. Compile the code:

   ```bash
   g++ main.cpp -o output
   ```
2. Run:

   ```bash
   ./output
   ```

---

## 📷 Example

Input: Source = A, Destination = D
Output: Shortest Path = A → B → D
Distance = X units

## 🔮 Future Improvements

* Add dynamic graph input
* Integrate map-based visualization
* Optimize for large-scale graphs

-
## 🤝 Acknowledgment

This project was developed with guidance and reference materials.
Focus was on understanding and implementing Dijkstra’s algorithm and deploying it as a working service.

---
