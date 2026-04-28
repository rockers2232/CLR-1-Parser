# ⚙️ CLR(1) C++ Parser Engine Pro

**An advanced, interactive web-based compiler automaton engineered to parse C++ syntax in real-time.**

## 📖 Overview

The **CLR(1) C++ Parser Engine** is a full-stack educational and diagnostic tool that bridges the gap between theoretical compiler design and practical implementation. It takes raw C++ source code, lexically analyzes it, and parses it through a custom-built CLR(1) bottom-up parsing algorithm. 

Featuring a modern, interactive dashboard, it visualizes the entire compilation pipeline—from calculating FIRST and FOLLOW sets to rendering the complete ACTION and GOTO matrices and executing a step-by-step memory stack trace.

## ✨ Key Features

* **Robust C++ Subset Grammar:** Successfully parses `#include` directives, namespaces, variable declarations, arithmetic expressions, `for` loops, `while` loops, and `if/else` control structures.
* **Intelligent Lexical Analyzer:** Tokenizes input streams while tracking exact line numbers for precise syntax error reporting.
* **Automated Automaton Generation:** Dynamically computes closures, `FIRST` sets, and `FOLLOW` sets based on a universal ruleset.
* **Real-Time Visualization:** * Interactive code editor with synchronized line numbering.
  * Live memory stack trace.
  * Complete, scrollable ACTION and GOTO matrices.
* **Modern UI/UX:** A sleek, dark-themed dashboard built with an intuitive tab-navigation system and glass-morphism elements.

---

## 💻 Tech Stack

| Component | Technology | Description |
| :--- | :--- | :--- |
| **Backend Engine** | C++ | Core CLR(1) algorithm, lexer, and closure logic. |
| **Middleware** | Python / Flask | RESTful API bridging the binary engine and the web UI. |
| **Frontend** | HTML, CSS, Vanilla JS | Responsive dashboard, dynamic table generation, DOM manipulation. |

---

## 🚀 Quick Start

### Prerequisites
* **Python 3.x**
* **g++ Compiler** (Must be added to your system PATH)

### Installation

```bash
git clone [https://github.com/rockers2232/CLR-1-Parser.git](https://github.com/rockers2232/CLR-1-Parser.git)
cd CLR-1-Parser
pip install flask
Execution
Bash
python app.py
Open your web browser and navigate to http://127.0.0.1:5000 to access the dashboard.

📂 Project Architecture
Plaintext
CLR-1-Parser/
├── app.py              
├── parser.cpp          
├── index.html          
└── README.md           
🧠 How It Works
Input Stage: The user enters C++ code into the web editor.

Transport: JavaScript fetches the input and sends it to the Flask backend via a POST request.

Compilation & Execution: Flask compiles parser.cpp (if not already compiled) and pipes the C++ code into the executable's standard input.

Lexical Analysis: The C++ engine tokenizes the string, mapping keywords, operators, and identifiers while tracking line numbers.

Table Construction: The engine calculates the grammar's FIRST/FOLLOW sets and builds the deterministic finite automaton (DFA) states to populate the ACTION and GOTO tables.

Parsing: The engine utilizes a stack to shift and reduce tokens according to the ACTION table.

Response: The backend formats the trace, tables, and status (Accept/Reject) into JSON and returns it to the frontend for rendering.

👨‍💻 Author
Ayush GitHub Profile
