# Inventory Management System

A desktop-based Inventory Management System built in **C** with a **GTK3 GUI**, 
developed as a final project for a Data Structures and Algorithms course.

---

## Project Overview

This system helps small businesses manage their inventory, customers, and orders 
efficiently using core data structures. It replaces manual spreadsheet tracking 
with a real-time desktop application.

---

## Features

### Inventory Module
- Add, delete, search, and update stock for products
- Data stored in a **Binary Search Tree (BST)** sorted by product ID
- Admin actions are logged and can be undone

### Customer Module
- Add and search customers
- Data stored in a **Hash Table** for O(1) average lookup

### Order Module
- Place and process orders using **FIFO Queue**
- Orders processed in the order they were received

### Admin Module
- View admin action log
- Undo last inventory action using a **Stack**

---

## Data Structures Used

| Module    | Data Structure | Reason                                      |
|-----------|---------------|----------------------------------------------|
| Inventory | BST           | O(log n) search, insert, delete              |
| Customers | Hash Table    | O(1) average lookup by customer ID           |
| Orders    | Queue (FIFO)  | Fair first-come-first-served order processing|
| Admin     | Stack (LIFO)  | Undo last action naturally                   |
