# TO-DO
- Melvin and Zhi Hang, help me fill in your own details.

# ECE3072 MINI PROJECT

## Project Description
Real-Time Embedded Vision Using Arm – Grove AI V2 and NIOS based Multitasking System on FPGA

## Team Members

| No. | Full Name | Student ID | Monash Email |
|------|-----------|------------|--------------|
| 1 | You Qing Liew | 33590400 | ylie0025@student.monash.edu |
| 2 | Melvin | [Fill in your student ID] | [Fill in your Monash email] |
| 3 | Zhi Hang | [Fill in your student ID] | [Fill in your Monash email] |
| 4 | [Name] | [Fill in your student ID] | [Fill in your Monash email] |



# Some SD practices

## Branching

* `main` → stable production-ready code
* All new work must be done in a separate branch, don't be stingy on creating new branches, spam it when u think it is neccessary
* No matter for fixing bugs, put in new features. This can make our works very easy to keep track and revert back if something unpleasant happened

Branch naming convention I suggested:

```
feature/<short-feature-description>
bugfix/<short-bug-description>
refactor/<code-improvement>
test/<testing-code>
```

Example:

```
feature/led-control
feature/button-input
bugfix/display-error
```

---

# Development Process

1. Pull the latest version of `main`

```
git checkout main
git pull origin main
```

2. Create a new branch

```
git checkout -b feature/led-control
```

3. Implement the feature and commit changes

4. Push the branch

```
git push origin feature/led-control
```

5. Create a **Pull Request (PR)**

6. Team members review the code before merging into `main`.

---

# Commit Message Convention

We follow **Conventional Commits**, which is widely used in industry.

Format:

```
<type>: <short description>
```

Common types:

```
feat: add new feature
fix: fix a bug
docs: documentation changes
refactor: improve code without changing behavior
test: add or update tests
chore: maintenance tasks
```

Examples:

```
feat: add LED control functionality
fix: correct pushbutton edge detection
docs: update README file
refactor: simplify comparator logic
```

---

# Code Review and Merging

* All new features must be submitted as a **Pull Request**
* At least **one team member reviews the code**
* After approval, the branch will be merged into `main`

---

# Good Practices

* Do not commit directly to `main`
* Always pull the latest `main` before starting new work
* Write meaningful commit messages
* Test your code before pushing



