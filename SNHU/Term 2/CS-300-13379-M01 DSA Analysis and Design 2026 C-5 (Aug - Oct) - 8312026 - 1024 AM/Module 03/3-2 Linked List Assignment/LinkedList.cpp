//============================================================================
// Name        : LinkedList.cpp
// Author      : Draven Chen
// Course      : CS 300 - Module Three - Linked List Assignment
// Description : Implements a singly linked list to hold Bid records loaded
//               from a municipal eBid CSV file. Supports appending,
//               prepending, printing, searching, and removing bids by ID,
//               driven from an interactive console menu.
//============================================================================

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

//============================================================================
// Global definitions
//============================================================================

// Represents a single bid record loaded from (or entered into) the CSV data.
// Kept as a plain struct so it stays a simple, reusable data-transfer object
// that is not tied to any particular container implementation.
struct Bid {
    string bidId;
    string title;
    string fund;
    double amount;

    Bid() : amount(0.0) {}
};

//============================================================================
// Linked-List class definition
//============================================================================

/**
 * Singly linked list container specialized to hold Bid records.
 *
 * The class is intentionally kept generic in its responsibilities: it knows
 * nothing about CSV files, the console, or menus. It only knows how to
 * store, traverse, search, and remove Bid nodes. That separation is what
 * keeps it modular and reusable in other programs.
 */
class LinkedList {

private:
    // Task 1: internal node structure used to build the list.
    // Kept private/internal to the class so callers never touch raw nodes -
    // they only ever interact with Bid values through the public API.
    struct Node {
        Bid bid;
        Node* next;

        // Default node has no payload yet and no successor.
        Node() {
            next = nullptr;
        }

        // Convenience constructor used by Append/Prepend so callers don't
        // have to manually wire up the "next" pointer every time.
        Node(Bid aBid) {
            bid = aBid;
            next = nullptr;
        }
    };

    // Task 1 (continued): housekeeping variables needed to manage the list
    // without having to walk it for every operation.
    Node* head;   // first node in the list (nullptr when empty)
    Node* tail;   // last node in the list, so Append() is O(1) not O(n)
    int size;     // running count of nodes, so callers can query Size()
                  // without traversing the whole list

public:
    LinkedList();
    virtual ~LinkedList();
    void Append(Bid bid);
    void Prepend(Bid bid);
    void PrintList();
    void Remove(string bidId);
    Bid Search(string bidId);
    int Size();
};

/**
 * Task 2: Default constructor.
 * Initializes the housekeeping variables so the list always starts in a
 * known, empty state (no dangling/garbage pointers).
 */
LinkedList::LinkedList() {
    head = nullptr;
    tail = nullptr;
    size = 0;
}

/**
 * Destructor - walks the list and frees every node.
 * Without this, every Bid loaded from a 12,000+ row CSV would leak memory
 * once the LinkedList object goes out of scope.
 */
LinkedList::~LinkedList() {
    Node* current = head;
    while (current != nullptr) {
        Node* temp = current;   // hold onto the node we're about to delete
        current = current->next; // advance before the delete invalidates it
        delete temp;
    }
    head = nullptr;
    tail = nullptr;
    size = 0;
}

/**
 * Task 3: Append a new bid to the end of the list.
 * Because we keep a tail pointer, this is O(1) instead of requiring a full
 * traversal to find the last node every time a bid is added.
 */
void LinkedList::Append(Bid bid) {
    Node* newNode = new Node(bid);

    if (head == nullptr) {
        // Empty list: the new node becomes both head and tail.
        head = newNode;
        tail = newNode;
    } else {
        // Non-empty list: link the current tail to the new node, then
        // advance the tail pointer.
        tail->next = newNode;
        tail = newNode;
    }
    size++;
}

/**
 * Task 4: Prepend a new bid to the front of the list.
 * This is always O(1) since we only ever touch the head pointer.
 */
void LinkedList::Prepend(Bid bid) {
    Node* newNode = new Node(bid);

    if (head == nullptr) {
        // Empty list: the new node becomes both head and tail.
        head = newNode;
        tail = newNode;
    } else {
        newNode->next = head;
        head = newNode;
    }
    size++;
}

/**
 * Task 5: Print every bid currently stored in the list, in list order.
 * Matches the "title | amount | fund" format shown in the assignment's
 * sample output.
 */
void LinkedList::PrintList() {
    Node* current = head;

    while (current != nullptr) {
        cout << current->bid.bidId << ": " << current->bid.title << " | "
             << current->bid.amount << " | " << current->bid.fund << endl;
        current = current->next;
    }
}

/**
 * Task 6: Remove the bid matching the given bidId, if it exists.
 * Handles three cases: removing the head, removing an interior/tail node,
 * and the "not found" case where nothing is removed.
 */
void LinkedList::Remove(string bidId) {
    // Case 1: the list is empty - nothing to remove.
    if (head == nullptr) {
        return;
    }

    // Case 2: the node to remove is the head.
    if (head->bid.bidId == bidId) {
        Node* temp = head;
        head = head->next;
        if (head == nullptr) {
            // The list is now empty, so tail must be reset too.
            tail = nullptr;
        }
        delete temp;
        size--;
        return;
    }

    // Case 3: search the rest of the list, keeping a trailing pointer so
    // we can re-link around the node we're deleting.
    Node* prev = head;
    Node* current = head->next;

    while (current != nullptr) {
        if (current->bid.bidId == bidId) {
            prev->next = current->next;
            if (current == tail) {
                // We removed the last node, so the trailing pointer becomes
                // the new tail.
                tail = prev;
            }
            delete current;
            size--;
            return;
        }
        prev = current;
        current = current->next;
    }
    // Case 4 (implicit): bidId was never found - list is left unchanged.
}

/**
 * Task 7: Search for a bid by ID.
 * Returns the matching Bid if found, or a default-constructed (empty) Bid
 * if no node matches - the caller can detect "not found" by checking
 * whether bidId is still empty.
 */
Bid LinkedList::Search(string bidId) {
    Node* current = head;

    while (current != nullptr) {
        if (current->bid.bidId == bidId) {
            return current->bid;
        }
        current = current->next;
    }

    // Not found: return a default Bid (empty bidId signals "no match").
    Bid emptyBid;
    return emptyBid;
}

/**
 * Returns the number of bids currently stored. Not part of the required
 * API, but a small, low-risk addition that other code (or future
 * assignments reusing this class) can rely on instead of re-implementing
 * a manual count.
 */
int LinkedList::Size() {
    return size;
}

//============================================================================
// Static (free) methods used for CSV parsing / demo purposes below
//============================================================================

/**
 * Splits one CSV line into fields, honoring double-quoted fields that may
 * contain commas (e.g. "Smith, John"). A hand-rolled splitter is used here
 * instead of assuming a fixed column layout, so the same function works for
 * both eBid_Monthly_Sales.csv and eBid_Monthly_Sales_Dec_2016.csv even if
 * their column order or quoting differs slightly.
 */
vector<string> splitCSVLine(const string& line) {
    vector<string> fields;
    string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];

        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(field);
            field.clear();
        } else {
            field += c;
        }
    }
    fields.push_back(field); // last field after the final comma

    return fields;
}

/**
 * Converts a string amount like "$1,234.56" into a double, stripping any
 * currency symbol and thousands separators first.
 */
double strToDouble(string str, char removeChar) {
    string cleaned;
    for (char c : str) {
        if (c != removeChar && c != ',') {
            cleaned += c;
        }
    }
    try {
        return stod(cleaned);
    } catch (...) {
        // Malformed/empty amount field - default to 0.0 rather than
        // crashing the whole load on a single bad row.
        return 0.0;
    }
}

/**
 * Lower-cases and trims a header string so column lookups are not sensitive
 * to case or stray whitespace (e.g. " Bid ID" vs "bid id").
 */
string normalizeHeader(string header) {
    // Trim leading/trailing whitespace.
    size_t start = header.find_first_not_of(" \t\r\n");
    size_t end = header.find_last_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }
    header = header.substr(start, end - start + 1);

    transform(header.begin(), header.end(), header.begin(), ::tolower);
    return header;
}

/**
 * Loads bids from the given CSV file path into the provided LinkedList.
 *
 * Rather than hard-coding column positions (which differ slightly between
 * the two provided CSV files), this reads the header row once and builds a
 * name -> column index map. That keeps loading correct regardless of column
 * order, and is the "challenge encountered" discussed in the reflection.
 */
void loadBids(const string& csvPath, LinkedList& bidList) {
    ifstream file(csvPath);

    if (!file.is_open()) {
        cout << "Could not open file: " << csvPath << endl;
        return;
    }

    string line;
    int bidCount = 0;

    // Read and parse the header row to locate each column we care about.
    if (!getline(file, line)) {
        cout << "File is empty: " << csvPath << endl;
        return;
    }

    vector<string> headers = splitCSVLine(line);
    int idCol = -1, titleCol = -1, fundCol = -1, amountCol = -1;

    for (size_t i = 0; i < headers.size(); ++i) {
        string h = normalizeHeader(headers[i]);
        if (h.find("bid id") != string::npos || h == "id") {
            idCol = static_cast<int>(i);
        } else if (h.find("title") != string::npos) {
            titleCol = static_cast<int>(i);
        } else if (h.find("fund") != string::npos) {
            fundCol = static_cast<int>(i);
        } else if (h.find("winning bid") != string::npos ||
                   h.find("amount") != string::npos) {
            amountCol = static_cast<int>(i);
        }
    }

    // Read every remaining row and append it as a Bid.
    while (getline(file, line)) {
        if (line.empty()) {
            continue; // skip blank lines rather than creating empty bids
        }

        vector<string> fields = splitCSVLine(line);
        Bid bid;

        if (idCol >= 0 && idCol < static_cast<int>(fields.size())) {
            bid.bidId = fields[idCol];
        }
        if (titleCol >= 0 && titleCol < static_cast<int>(fields.size())) {
            bid.title = fields[titleCol];
        }
        if (fundCol >= 0 && fundCol < static_cast<int>(fields.size())) {
            bid.fund = fields[fundCol];
        }
        if (amountCol >= 0 && amountCol < static_cast<int>(fields.size())) {
            bid.amount = strToDouble(fields[amountCol], '$');
        }

        bidList.Append(bid);
        bidCount++;
    }

    file.close();
    cout << bidCount << " bids read" << endl;
}

/**
 * Prints a single bid in the "id: title | amount | fund" format used
 * elsewhere in the program, so Find/Enter share one display routine
 * instead of duplicating the formatting logic.
 */
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
         << bid.fund << endl;
}

/**
 * Prompts the user for each Bid field on the console and returns the
 * completed Bid. Kept separate from the menu loop so input-gathering has a
 * single responsibility and can be reused/tested independently.
 */
Bid getBidInfo() {
    Bid bid;
    string strAmount;

    cout << "Enter Id: ";
    cin.ignore();
    getline(cin, bid.bidId);

    cout << "Enter title: ";
    getline(cin, bid.title);

    cout << "Enter fund: ";
    getline(cin, bid.fund);

    cout << "Enter amount: ";
    cin >> strAmount;
    bid.amount = strToDouble(strAmount, '$');

    return bid;
}

//============================================================================
// Main driver: presents the menu and dispatches to the LinkedList methods
//============================================================================

int main(int argc, char* argv[]) {

    // Allow the CSV path to be overridden from the command line; otherwise
    // default to the larger data set as described in the assignment.
    string csvPath;
    switch (argc) {
        case 2:
            csvPath = argv[1];
            break;
        default:
            csvPath = "eBid_Monthly_Sales.csv";
    }

    LinkedList bidList;
    Bid bid;

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Enter a Bid" << endl;
        cout << "  2. Load Bids" << endl;
        cout << "  3. Display All Bids" << endl;
        cout << "  4. Find Bid" << endl;
        cout << "  5. Remove Bid" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:
            bid = getBidInfo();
            bidList.Append(bid);
            displayBid(bid);
            break;

        case 2: {
            cout << "Loading CSV file " << csvPath << endl;

            // Time the load using clock ticks, matching the sample output
            // format shown in the assignment.
            clock_t ticks = clock();
            loadBids(csvPath, bidList);
            ticks = clock() - ticks;

            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds"
                 << endl;
            break;
        }

        case 3:
            bidList.PrintList();
            break;

        case 4: {
            string bidKey;
            cout << "Enter bid Id: ";
            cin >> bidKey;

            clock_t ticks = clock();
            bid = bidList.Search(bidKey);
            ticks = clock() - ticks;

            if (!bid.bidId.empty()) {
                displayBid(bid);
            } else {
                cout << "Bid Id " << bidKey << " not found." << endl;
            }

            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds"
                 << endl;
            break;
        }

        case 5: {
            string bidKey;
            cout << "Enter bid Id: ";
            cin >> bidKey;
            bidList.Remove(bidKey);
            break;
        }

        default:
            if (choice != 9) {
                cout << choice << " is not a valid option." << endl;
            }
            break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
