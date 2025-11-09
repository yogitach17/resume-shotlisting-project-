#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <queue>

using namespace std;

class Candidate {
private:
    string name;
    double cgpa;
    int experience; 
    vector<string> skills;

public:
    
    Candidate(string n = "", double c = 0.0, int exp = 0, vector<string> sk = {})
        : name(n), cgpa(c), experience(exp), skills(sk) {}

    string getName() const { return name; }
    double getCGPA() const { return cgpa; }
    int getExperience() const { return experience; }
    vector<string> getSkills() const { return skills; }
    int getSkillCount() const { return skills.size(); }

    void setName(string n) { name = n; }
    void setCGPA(double c) { cgpa = c; }
    void setExperience(int exp) { experience = exp; }
    void setSkills(vector<string> sk) { skills = sk; }

    void display() const {
        cout << left << setw(20) << name 
             << setw(8) << fixed << setprecision(2) << cgpa
             << setw(8) << experience << "yr"
             << setw(8) << skills.size() << "   ";
   
        for (size_t i = 0; i < skills.size(); i++) {
            cout << skills[i];
            if (i < skills.size() - 1) cout << ", ";
        }
        cout << endl;
    }

    string serialize() const {
        stringstream ss;
        ss << name << "|" << cgpa << "|" << experience << "|";
        for (size_t i = 0; i < skills.size(); i++) {
            ss << skills[i];
            if (i < skills.size() - 1) ss << ",";
        }
        return ss.str();
    }

    static Candidate deserialize(const string& line) {
        stringstream ss(line);
        string name, cgpaStr, expStr, skillsStr;
        
        getline(ss, name, '|');
        getline(ss, cgpaStr, '|');
        getline(ss, expStr, '|');
        getline(ss, skillsStr);

        double cgpa = stod(cgpaStr);
        int exp = stoi(expStr);

        vector<string> skills;
        stringstream skillStream(skillsStr);
        string skill;
        while (getline(skillStream, skill, ',')) {
            skills.push_back(skill);
        }

        return Candidate(name, cgpa, exp, skills);
    }
};

class TechnicalCandidate : public Candidate {
private:
    string primaryLanguage;

public:
    TechnicalCandidate(string n, double c, int exp, vector<string> sk, string lang)
        : Candidate(n, c, exp, sk), primaryLanguage(lang) {}

    string getPrimaryLanguage() const { return primaryLanguage; }
    void setPrimaryLanguage(string lang) { primaryLanguage = lang; }
};

class ResumeManager {
private:
    vector<Candidate> candidates;
    const string filename = "resumes.txt";

    void merge(vector<Candidate>& arr, int left, int mid, int right, bool (*compare)(const Candidate&, const Candidate&)) {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        vector<Candidate> L(n1), R(n2);

        for (int i = 0; i < n1; i++)
            L[i] = arr[left + i];
        for (int j = 0; j < n2; j++)
            R[j] = arr[mid + 1 + j];

        int i = 0, j = 0, k = left;

        while (i < n1 && j < n2) {
            if (compare(L[i], R[j])) {
                arr[k] = L[i];
                i++;
            } else {
                arr[k] = R[j];
                j++;
            }
            k++;
        }

        while (i < n1) {
            arr[k] = L[i];
            i++;
            k++;
        }

        while (j < n2) {
            arr[k] = R[j];
            j++;
            k++;
        }
    }

    void mergeSort(vector<Candidate>& arr, int left, int right, bool (*compare)(const Candidate&, const Candidate&)) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSort(arr, left, mid, compare);
            mergeSort(arr, mid + 1, right, compare);
            merge(arr, left, mid, right, compare);
        }
    }

    void insertionSort(vector<Candidate>& arr, bool (*compare)(const Candidate&, const Candidate&)) {
        for (size_t i = 1; i < arr.size(); i++) {
            Candidate key = arr[i];
            int j = i - 1;

            while (j >= 0 && compare(key, arr[j])) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }

    static bool compareByCGPA(const Candidate& a, const Candidate& b) {
        return a.getCGPA() > b.getCGPA();  // Descending
    }

    static bool compareByExperience(const Candidate& a, const Candidate& b) {
        return a.getExperience() > b.getExperience();  // Descending
    }

    static bool compareBySkills(const Candidate& a, const Candidate& b) {
        return a.getSkillCount() > b.getSkillCount();  // Descending
    }

    static bool compareByName(const Candidate& a, const Candidate& b) {
        return a.getName() < b.getName();  // Ascending for binary search
    }

public:
   
    void addCandidate() {
        string name;
        double cgpa;
        int exp;
        int numSkills;
        vector<string> skills;

        cout << "\n--- Add Candidate ---\n";
        cout << "Enter Name: ";
        cin.ignore();
        getline(cin, name);

        cout << "Enter CGPA (0.0 - 10.0): ";
        cin >> cgpa;

        cout << "Enter Experience (years): ";
        cin >> exp;

        cout << "Enter number of skills: ";
        cin >> numSkills;

        cout << "Enter skills (one per line):\n";
        cin.ignore();
        for (int i = 0; i < numSkills; i++) {
            string skill;
            getline(cin, skill);
            skills.push_back(skill);
        }

        candidates.push_back(Candidate(name, cgpa, exp, skills));
        cout << "\n✓ Candidate added successfully!\n";
    }

    void displayAll() const {
        if (candidates.empty()) {
            cout << "\nNo candidates available.\n";
            return;
        }

        cout << "\n" << string(80, '=') << endl;
        cout << "                    ALL CANDIDATES\n";
        cout << string(80, '=') << endl;
        cout << left << setw(20) << "Name" 
             << setw(8) << "CGPA"
             << setw(8) << "Exp"
             << setw(8) << "Skills" << "  Skill List\n";
        cout << string(80, '-') << endl;

        for (const auto& candidate : candidates) {
            candidate.display();
        }
        cout << string(80, '=') << endl;
    }
  
    void sortCandidates(int choice) {
        if (candidates.empty()) {
            cout << "\nNo candidates to sort.\n";
            return;
        }

        switch (choice) {
            case 1:  // Sort by CGPA (Merge Sort)
                mergeSort(candidates, 0, candidates.size() - 1, compareByCGPA);
                cout << "\n✓ Sorted by CGPA (Descending) using Merge Sort\n";
                break;
            case 2:  // Sort by Experience (Merge Sort)
                mergeSort(candidates, 0, candidates.size() - 1, compareByExperience);
                cout << "\n✓ Sorted by Experience (Descending) using Merge Sort\n";
                break;
            case 3:  // Sort by Skills (Insertion Sort)
                insertionSort(candidates, compareBySkills);
                cout << "\n✓ Sorted by Skills Count (Descending) using Insertion Sort\n";
                break;
        }
        displayAll();
    }
    
    void searchByName() {
        if (candidates.empty()) {
            cout << "\nNo candidates available.\n";
            return;
        }

        string name;
        cout << "\nEnter candidate name to search: ";
        cin.ignore();
        getline(cin, name);
       
        vector<Candidate> sortedCandidates = candidates;
        mergeSort(sortedCandidates, 0, sortedCandidates.size() - 1, compareByName);
        
        int left = 0, right = sortedCandidates.size() - 1;
        bool found = false;

        while (left <= right) {
            int mid = left + (right - left) / 2;
            string midName = sortedCandidates[mid].getName();

            if (midName == name) {
                cout << "\n✓ Candidate Found (Binary Search):\n";
                cout << string(80, '-') << endl;
                cout << left << setw(20) << "Name" << setw(8) << "CGPA"
                     << setw(8) << "Exp" << setw(8) << "Skills" << "  Skill List\n";
                cout << string(80, '-') << endl;
                sortedCandidates[mid].display();
                found = true;
                break;
            } else if (midName < name) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        if (!found) {
            cout << "\n✗ Candidate not found.\n";
        }
    }
    
    void searchBySkill() {
        if (candidates.empty()) {
            cout << "\nNo candidates available.\n";
            return;
        }

        string skill;
        cout << "\nEnter skill to search: ";
        cin.ignore();
        getline(cin, skill);

        vector<Candidate> results;

        // Linear Search through all candidates
        for (const auto& candidate : candidates) {
            vector<string> candidateSkills = candidate.getSkills();
            for (const auto& s : candidateSkills) {
                if (s == skill) {
                    results.push_back(candidate);
                    break;
                }
            }
        }

        if (results.empty()) {
            cout << "\n✗ No candidates found with skill: " << skill << endl;
        } else {
            cout << "\n✓ Found " << results.size() << " candidate(s) with skill '" << skill << "' (Linear Search):\n";
            cout << string(80, '-') << endl;
            cout << left << setw(20) << "Name" << setw(8) << "CGPA"
                 << setw(8) << "Exp" << setw(8) << "Skills" << "  Skill List\n";
            cout << string(80, '-') << endl;
            for (const auto& candidate : results) {
                candidate.display();
            }
        }
    }
   
    void showTop3() {
        if (candidates.empty()) {
            cout << "\nNo candidates available.\n";
            return;
        }

        cout << "\n--- Top 3 Candidates ---\n";
        cout << "1. By CGPA\n2. By Experience\nEnter choice: ";
        int choice;
        cin >> choice;

        // Max heap using priority queue
        auto cgpaComparator = [](const Candidate& a, const Candidate& b) {
            return a.getCGPA() < b.getCGPA();  // Max heap
        };

        auto expComparator = [](const Candidate& a, const Candidate& b) {
            return a.getExperience() < b.getExperience();  // Max heap
        };

        if (choice == 1) {
            priority_queue<Candidate, vector<Candidate>, decltype(cgpaComparator)> maxHeap(cgpaComparator);
            for (const auto& candidate : candidates) {
                maxHeap.push(candidate);
            }

            cout << "\n✓ Top 3 Candidates by CGPA (using Max Heap):\n";
            cout << string(80, '-') << endl;
            cout << left << setw(20) << "Name" << setw(8) << "CGPA"
                 << setw(8) << "Exp" << setw(8) << "Skills" << "  Skill List\n";
            cout << string(80, '-') << endl;

            int count = 0;
            while (!maxHeap.empty() && count < 3) {
                maxHeap.top().display();
                maxHeap.pop();
                count++;
            }
        } else if (choice == 2) {
            priority_queue<Candidate, vector<Candidate>, decltype(expComparator)> maxHeap(expComparator);
            for (const auto& candidate : candidates) {
                maxHeap.push(candidate);
            }

            cout << "\n✓ Top 3 Candidates by Experience (using Max Heap):\n";
            cout << string(80, '-') << endl;
            cout << left << setw(20) << "Name" << setw(8) << "CGPA"
                 << setw(8) << "Exp" << setw(8) << "Skills" << "  Skill List\n";
            cout << string(80, '-') << endl;

            int count = 0;
            while (!maxHeap.empty() && count < 3) {
                maxHeap.top().display();
                maxHeap.pop();
                count++;
            }
        }
    }
    
    void saveToFile() {
        ofstream outFile(filename);
        if (!outFile) {
            cout << "\n✗ Error: Unable to save to file.\n";
            return;
        }

        for (const auto& candidate : candidates) {
            outFile << candidate.serialize() << endl;
        }

        outFile.close();
        cout << "\n✓ Data saved to " << filename << endl;
    }
    
    void loadFromFile() {
        ifstream inFile(filename);
        if (!inFile) {
            cout << "\nNo existing data file found. Starting fresh.\n";
            return;
        }

        candidates.clear();
        string line;
        int count = 0;

        while (getline(inFile, line)) {
            if (!line.empty()) {
                candidates.push_back(Candidate::deserialize(line));
                count++;
            }
        }

        inFile.close();
        cout << "\n✓ Loaded " << count << " candidate(s) from " << filename << endl;
    }
};

int main() {
    ResumeManager manager;
    manager.loadFromFile(); 

    int choice;
    do {
        cout << "\n" << string(50, '=') << endl;
        cout << "     RESUME SHORTLISTING SYSTEM\n";
        cout << string(50, '=') << endl;
        cout << "1.  Add Candidate\n";
        cout << "2.  Display All Candidates\n";
        cout << "3.  Sort by CGPA\n";
        cout << "4.  Sort by Experience\n";
        cout << "5.  Sort by Skills Count\n";
        cout << "6.  Search by Name (Binary Search)\n";
        cout << "7.  Search by Skill (Linear Search)\n";
        cout << "8.  Show Top 3 Candidates\n";
        cout << "9.  Save & Exit\n";
        cout << string(50, '=') << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                manager.addCandidate();
                break;
            case 2:
                manager.displayAll();
                break;
            case 3:
                manager.sortCandidates(1);
                break;
            case 4:
                manager.sortCandidates(2);
                break;
            case 5:
                manager.sortCandidates(3);
                break;
            case 6:
                manager.searchByName();
                break;
            case 7:
                manager.searchBySkill();
                break;
            case 8:
                manager.showTop3();
                break;
            case 9:
                manager.saveToFile();
                cout << "\nThank you for using Resume Sorter & Ranker!\n";
                break;
            default:
                cout << "\n✗ Invalid choice. Please try again.\n";
        }
    } while (choice != 9);
    return 0;
}
