/*
 * Name: Muhammad Nouman Shakeel
 * Registration No: 546177
 * Section: BSAI-2A
 * Course: CS - Data Structures and Algorithms
 * Open Ended Lab: Student Help Desk
 *
 * Design choice:
 * - Students are stored in a Binary Search Tree (BST) keyed by unique roll number.
 *   A BST fits this desk because rolls are unique identifiers and the common desk
 *   operations are insert, search-by-roll, and ordered display. Average insert and
 *   search are O(log n), which is better than scanning a linked list for each query.
 * - Help requests use a linked Queue of roll numbers (FIFO). The earliest waiting
 *   student is served first, which matches a real help-desk counter. The queue only
 *   stores rolls (not full student records), so serving a request never deletes the
 *   student from the BST.
 *
 * Complexity (worst case on a skewed BST):
 * - Add student:    O(n)
 * - Search student: O(n)
 * Average case on a reasonably balanced tree is O(log n) for both.
 * Queue enqueue / dequeue: O(1)
 */

#include <iostream>
#include <cstring>
using namespace std;

const int NAME_LEN = 64;

// -------------------- Student BST --------------------

struct StudentNode {
    int roll;
    char name[NAME_LEN];
    StudentNode* left;
    StudentNode* right;

    StudentNode(int r, const char* n) {
        roll = r;
        strncpy(name, n, NAME_LEN - 1);
        name[NAME_LEN - 1] = '\0';
        left = right = 0;
    }
};

StudentNode* insertStudent(StudentNode* root, int roll, const char* name, bool& inserted) {
    if (root == 0) {
        inserted = true;
        return new StudentNode(roll, name);
    }
    if (roll == root->roll) {
        inserted = false; // duplicate
        return root;
    }
    if (roll < root->roll)
        root->left = insertStudent(root->left, roll, name, inserted);
    else
        root->right = insertStudent(root->right, roll, name, inserted);
    return root;
}

StudentNode* searchStudent(StudentNode* root, int roll) {
    if (root == 0)
        return 0;
    if (roll == root->roll)
        return root;
    if (roll < root->roll)
        return searchStudent(root->left, roll);
    return searchStudent(root->right, roll);
}

void displayStudents(StudentNode* root) {
    if (root == 0)
        return;
    displayStudents(root->left);
    cout << "  Roll: " << root->roll << "    Name: " << root->name << endl;
    displayStudents(root->right);
}

int countStudents(StudentNode* root) {
    if (root == 0)
        return 0;
    return 1 + countStudents(root->left) + countStudents(root->right);
}

void freeTree(StudentNode* root) {
    if (root == 0)
        return;
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

// -------------------- Help-request Queue --------------------

struct QueueNode {
    int roll;
    QueueNode* next;

    QueueNode(int r) {
        roll = r;
        next = 0;
    }
};

struct HelpQueue {
    QueueNode* front;
    QueueNode* rear;

    HelpQueue() {
        front = rear = 0;
    }
};

bool isQueueEmpty(HelpQueue& q) {
    return q.front == 0;
}

void enqueueRequest(HelpQueue& q, int roll) {
    QueueNode* node = new QueueNode(roll);
    if (q.rear == 0) {
        q.front = q.rear = node;
    } else {
        q.rear->next = node;
        q.rear = node;
    }
}

bool dequeueRequest(HelpQueue& q, int& rollOut) {
    if (isQueueEmpty(q))
        return false;
    QueueNode* temp = q.front;
    rollOut = temp->roll;
    q.front = q.front->next;
    if (q.front == 0)
        q.rear = 0;
    delete temp;
    return true;
}

void displayQueue(HelpQueue& q) {
    if (isQueueEmpty(q)) {
        cout << "Help queue is empty.\n";
        return;
    }
    cout << "Waiting help requests (front -> rear):\n";
    QueueNode* cur = q.front;
    int pos = 1;
    while (cur != 0) {
        cout << "  " << pos << ") Roll " << cur->roll << endl;
        cur = cur->next;
        pos++;
    }
}

void freeQueue(HelpQueue& q) {
    int dummy;
    while (dequeueRequest(q, dummy)) {
        // nodes deleted inside dequeueRequest
    }
}

// -------------------- Menu helpers --------------------

void printMenu() {
    cout << "\n========== Student Help Desk ==========\n";
    cout << "1. Add student\n";
    cout << "2. Search student by roll number\n";
    cout << "3. Display all students (roll order)\n";
    cout << "4. Add help request (enqueue)\n";
    cout << "5. Serve next help request (dequeue)\n";
    cout << "6. Show help queue\n";
    cout << "7. Exit\n";
    cout << "Enter choice: ";
}

void addStudentMenu(StudentNode*& root) {
    int roll;
    char name[NAME_LEN];
    cout << "Enter roll number: ";
    cin >> roll;
    cin.ignore(10000, '\n');
    cout << "Enter name: ";
    cin.getline(name, NAME_LEN);

    bool inserted = false;
    root = insertStudent(root, roll, name, inserted);
    if (inserted)
        cout << "Student added successfully.\n";
    else
        cout << "Duplicate roll number. Student NOT added.\n";
}

void searchStudentMenu(StudentNode* root) {
    int roll;
    cout << "Enter roll number to search: ";
    cin >> roll;
    StudentNode* found = searchStudent(root, roll);
    if (found == 0)
        cout << "Search failed: no student with roll " << roll << ".\n";
    else
        cout << "Found -> Roll: " << found->roll << ", Name: " << found->name << endl;
}

void addHelpRequestMenu(StudentNode* root, HelpQueue& q) {
    int roll;
    cout << "Enter registered roll number for help request: ";
    cin >> roll;
    if (searchStudent(root, roll) == 0) {
        cout << "Rejected: roll " << roll << " is not registered.\n";
        return;
    }
    enqueueRequest(q, roll);
    cout << "Help request added for roll " << roll << ".\n";
}

void serveHelpRequestMenu(StudentNode* root, HelpQueue& q) {
    int roll;
    if (!dequeueRequest(q, roll)) {
        cout << "Cannot serve: help queue is empty.\n";
        return;
    }
    StudentNode* s = searchStudent(root, roll);
    cout << "Serving earliest request -> Roll: " << roll;
    if (s != 0)
        cout << ", Name: " << s->name;
    cout << endl;
}

int main() {
    StudentNode* root = 0;
    HelpQueue helpQ;
    int choice;

    cout << "Student Help Desk - Open Ended Lab\n";
    cout << "Muhammad Nouman Shakeel | 546177 | BSAI-2A\n";

    do {
        printMenu();
        cin >> choice;

        switch (choice) {
        case 1:
            addStudentMenu(root);
            break;
        case 2:
            searchStudentMenu(root);
            break;
        case 3:
            if (root == 0)
                cout << "No students registered yet.\n";
            else {
                cout << "All students (ascending roll):\n";
                displayStudents(root);
                cout << "Total students: " << countStudents(root) << endl;
            }
            break;
        case 4:
            addHelpRequestMenu(root, helpQ);
            break;
        case 5:
            serveHelpRequestMenu(root, helpQ);
            break;
        case 6:
            displayQueue(helpQ);
            break;
        case 7:
            cout << "Releasing memory and exiting...\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 7);

    freeQueue(helpQ);
    freeTree(root);
    cout << "Goodbye.\n";
    return 0;
}
