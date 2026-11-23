#include <iostream>
#include <vector>
#include <ctime>
#include<limits>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <algorithm>
using namespace std;
using Matrix = vector<vector<int>>;
using AdjList = vector<vector<int>>;

struct Graph {
    Matrix M;
    AdjList L;
    string name;
};

int Get(const Matrix &M, size_t i, size_t j) {
    return (i < M.size() && j < M.size()) ? M[i][j] : 0;
}

void Examination(int &n) {
    while (true) {
        if (cin >> n && n >= 1) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
        cout << "Ошибка: нужно целое число >= 1" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int ReadVertex(const string &prompt, size_t n) {
    int x;
    while (true) {
        cout << prompt;
        if (cin >> x && x >= 0 && static_cast<size_t>(x) < n) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return x;
        }
        cout << "Ошибка: нужно целое число от 0 до " << n-1 << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

    }
}
int ReadInt(const string &prompt, int min, int max) {
    int x;
    while (true) {
        cout << prompt;
        if (cin >> x && x >= min && x <= max) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return x;
        }
        cout << "Ошибка: нужно целое число от " << min << " до " << max << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

const Graph &ChooseGraph(const vector<Graph> &graphs) {
    int g = ReadInt("Выберите Граф (1 = G1; 2 = G2): ", 1, static_cast<int>(graphs.size()));
    return graphs[g-1];
}

bool HasLoop(const AdjList &L, size_t u, size_t v) {
    return find(L[u].begin(), L[u].end(), static_cast<int>(v)) != L[u].end();
}

void GenerateMatrixAdjacency(Matrix &M, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            M[i][j] = rand() % 2;
            M[j][i] = M[i][j];
        }
    }
}
AdjList AdjacencyList(const Matrix &M) {
    AdjList arr(M.size());
    for (size_t i = 0; i < M.size(); i++) {
        for (size_t j = 0; j < M[i].size(); j++) {
            if (M[i][j] == 1) {
                arr[i].push_back(j);
            }
        }
    }
    return arr;
}

Matrix VertexIdentification(const Matrix &M, size_t u, size_t v, bool makeloop){
    vector<int> Q(M.size(), 0);
    for (size_t i = 0; i < M.size(); i++) {
        Q[i] = M[u][i] | M[v][i];
    }
    vector<size_t> keep;
    for (size_t i = 0; i < M.size(); i++) {
        if (i != u && i != v) keep.push_back(i);
    }
    size_t k =keep.size();
    Matrix R(k+1,vector<int>(k+1,0) );
    for (size_t i = 0; i < k; i++) {
        for (size_t j = 0; j < k; j++) {
            R[i][j] = M[keep[i]][keep[j]];
        }
    }
    for (size_t i = 0; i < k; i++) {
        if (Q[keep[i]] == 1) {
            R[k][i] = 1;
            R[i][k] = 1;
        }
    }
    bool loop = M[u][u] == 1 || M[v][v] == 1 || makeloop && M[u][v] == 1;
    if (loop) {
        R[k][k] = 1;
    }
    return R;
}

AdjList VertexIdentificationList(const AdjList &L, size_t u, size_t v,bool makeloop) {
    vector<size_t> Q(L.size(), 0);
    for (size_t w : L[u]) Q[w] = 1;
    for (size_t w : L[v]) Q[w] = 1;
    vector<size_t> keep;
    for (size_t i = 0; i < L.size(); i++) {
        if (i != u && i != v) keep.push_back(i);
    }
    size_t k = keep.size();
    vector<int> newIndex(L.size(),-1);
    AdjList R(k + 1);

    for (size_t j = 0; j < k; j++) newIndex[keep[j]] = static_cast<int>(j);
    for (size_t j = 0; j < k ;j++) {
        size_t old = keep[j];

        for (size_t w : L[old]) {
            if (newIndex[w] != -1) {
                R[j].push_back(newIndex[w]);
            }
        }
        if (Q[old] == 1) {
            R[j].push_back(k);
            R[k].push_back(j);
        }
    }
    bool loop = HasLoop(L, u, u) || HasLoop(L, v, v) || makeloop && HasLoop(L, u, v);
    if (loop) {
        R[k].push_back(k);
    }
    return R;
    
}

Matrix VertexSplitting (const Matrix &M, size_t u) {
    vector<int> Q(M.size(), 0);
    size_t cnt = 0;
    for (size_t i = 0; i < M.size(); i++) {
        if (u == i) {
            continue;
        }
        Q[i] = M[u][i];
    }
    vector<size_t> keep;
    for (size_t i = 0; i < M.size(); i++) {
        if (i != u) keep.push_back(i);
    }
    size_t k = keep.size();
    Matrix R(k+2, vector<int>(k+2,0) );
    for (size_t i = 0; i < k; i++) {
        for (size_t j = 0; j < k; j++) {
            R[i][j] = M[keep[i]][keep[j]];
        }
    }
    R[k][k] = M[u][u];
    for (size_t i = 0; i < k; i++) {
        if (Q[keep[i]] == 1) {
            if (cnt % 2 == 0) {
                R[k][i] = 1;
                R[i][k] = 1;

            } else {
                R[k+1][i] = 1;
                R[i][k+1] = 1;
            }
            cnt++;
        }
    }
    R[k+1][k] = R[k][k+1] = 1;

    return R;
}

AdjList VertexSplittingList(const AdjList &L, size_t u) {
    vector<size_t> keep;
    size_t count = 0;
    for (size_t i = 0; i < L.size(); i++) {
        if (i != u) keep.push_back(i);
    }
    size_t k = keep.size();
    vector<int> newIndex(L.size(),-1);
    for (size_t j = 0; j < k; j++) newIndex[keep[j]] = static_cast<int>(j);
    AdjList R(k + 2);
    for (size_t j = 0; j < k; j++) {
        size_t old = keep[j];
        for (size_t w : L[old]) {
            if (newIndex[w] != -1) R[j].push_back(newIndex[w]);
        }
    }
    for (size_t w : L[u]) {
        if (w == u) continue;
        size_t target = (count % 2 == 0) ? k : k + 1;
        size_t j = newIndex[w];
        R[j].push_back(static_cast<int>(target));
        R[target].push_back(static_cast<int>(j));
        count++;
    }
    if (HasLoop(L, u, u)) R[k].push_back(static_cast<int>(k));
    R[k].push_back(static_cast<int>(k + 1));
    R[k + 1].push_back(static_cast<int>(k));
    return R;
}

// Объединение: V = V1 ∪ V2, ребро есть хотя бы в одном графе
Matrix GraphUnion(const Matrix &A, const Matrix &B) {
    size_t n = max(A.size(), B.size());
    Matrix R(n, vector<int>(n, 0));
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            R[i][j] = Get(A, i, j) | Get(B, i, j);
        }
    }
    return R;
}

// Пересечение: V = V1 ∩ V2, ребро есть в обоих графах
Matrix GraphIntersection(const Matrix &A, const Matrix &B) {
    size_t n = min(A.size(), B.size());
    Matrix R(n, vector<int>(n, 0));
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            R[i][j] = A[i][j] & B[i][j];
        }
    }
    return R;
}

// Кольцевая сумма: рёбра, которые есть ровно в одном из графов,
// затем изолированные вершины удаляются.
// В keep возвращаются старые номера оставшихся вершин (keep[новый] = старый).
Matrix GraphRingSum(const Matrix &A, const Matrix &B, vector<size_t> &keep) {
    size_t n = max(A.size(), B.size());
    Matrix S(n, vector<int>(n, 0));
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            S[i][j] = Get(A, i, j) ^ Get(B, i, j);
        }
    }

    keep.clear();
    for (size_t i = 0; i < n; i++) {
        bool isolated = true;
        for (size_t j = 0; j < n; j++) {
            if (S[i][j] == 1) {
                isolated = false;
                break;
            }
        }
        if (!isolated) keep.push_back(i);
    }

    size_t k = keep.size();
    Matrix R(k, vector<int>(k, 0));
    for (size_t i = 0; i < k; i++) {
        for (size_t j = 0; j < k; j++) {
            R[i][j] = S[keep[i]][keep[j]];
        }
    }
    return R;
}

bool HasEdge(const AdjList &L, size_t u, size_t v) {
    return u < L.size() &&
           find(L[u].begin(), L[u].end(), static_cast<int>(v)) != L[u].end();
}

void AddUnique(AdjList &L, size_t u, int v) {
    if (find(L[u].begin(), L[u].end(), v) == L[u].end()) {
        L[u].push_back(v);
    }
}

// Объединение списков смежности: ребро есть хотя бы в одном списке.
AdjList GraphUnionList(const AdjList &A, const AdjList &B) {
    size_t n = max(A.size(), B.size());
    AdjList R(n);
    for (size_t i = 0; i < n; i++) {
        if (i < A.size()) {
            for (int v : A[i]) {
                if (v >= 0 && static_cast<size_t>(v) < n) AddUnique(R, i, v);
            }
        }
        if (i < B.size()) {
            for (int v : B[i]) {
                if (v >= 0 && static_cast<size_t>(v) < n) AddUnique(R, i, v);
            }
        }
    }
    return R;
}

// Пересечение списков смежности: ребро есть в обоих списках.
AdjList GraphIntersectionList(const AdjList &A, const AdjList &B) {
    size_t n = min(A.size(), B.size());
    AdjList R(n);
    for (size_t i = 0; i < n; i++) {
        for (int v : A[i]) {
            if (v >= 0 && static_cast<size_t>(v) < n &&
                HasEdge(B, i, static_cast<size_t>(v))) {
                AddUnique(R, i, v);
            }
        }
    }
    return R;
}

// Кольцевая сумма списков смежности: ребро есть ровно в одном списке.
// Изолированные вершины удаляются, а в keep сохраняются их старые номера.
AdjList GraphRingSumList(const AdjList &A, const AdjList &B,
                         vector<size_t> &keep) {
    size_t n = max(A.size(), B.size());
    AdjList S(n);
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (HasEdge(A, i, j) != HasEdge(B, i, j)) {
                S[i].push_back(static_cast<int>(j));
            }
        }
    }

    keep.clear();
    for (size_t i = 0; i < n; i++) {
        if (!S[i].empty()) keep.push_back(i);
    }

    vector<int> newIndex(n, -1);
    for (size_t i = 0; i < keep.size(); i++) {
        newIndex[keep[i]] = static_cast<int>(i);
    }

    AdjList R(keep.size());
    for (size_t i = 0; i < keep.size(); i++) {
        for (int v : S[keep[i]]) {
            if (v >= 0 && newIndex[static_cast<size_t>(v)] != -1) {
                R[i].push_back(newIndex[static_cast<size_t>(v)]);
            }
        }
    }
    return R;
}

// Декартово произведение: вершина (a, b) имеет номер a * n2 + b
Matrix CartesianProduct(const Matrix &A, const Matrix &B) {
    size_t n1 = A.size(), n2 = B.size();
    Matrix R(n1 * n2, vector<int>(n1 * n2, 0));
    for (size_t a = 0; a < n1; a++) {
        for (size_t b = 0; b < n2; b++) {
            for (size_t c = 0; c < n1; c++) {
                for (size_t d = 0; d < n2; d++) {
                    bool adjacent = (a == c && B[b][d] == 1) || (b == d && A[a][c] == 1);
                    R[a * n2 + b][c * n2 + d] = adjacent ? 1 : 0;
                }
            }
        }
    }
    return R;
}

void PrintMatrix(const Matrix &M, const string &name){
    cout << "Матрица смежности " << name << endl;
    int w = to_string(M.size() -1 ).size() + 1;
    cout << setw(w+1) << "";
    for (size_t j = 0; j < M.size(); j++) {
        cout << setw(w) << j;
    }
        cout << endl;
    for (size_t i = 0; i < M.size(); i++) {
        cout << setw(w) << i << ":";
        for (size_t j = 0; j < M[i].size(); j++) {
            cout << setw(w) << M[i][j];
        }
        cout << endl;
    }
}
void PrintAdjacencyList(const AdjList &arr, const string &name) {
    cout << "Список смежности " << name << endl;
    for (size_t i = 0; i < arr.size(); i++) {
        cout << i << ": ";
        for (size_t j = 0; j < arr[i].size(); j++) {
            cout << arr[i][j] << " ";
        }
        cout << endl;
    }
}
int main() {
    srand(time(NULL));
    int n1, n2;
    cout << "Введите размер графа G1 (целое число): ";
    Examination(n1);
    cout << "Введите размер графа G2 (целое число): ";
    Examination(n2);
    Matrix M1(n1, vector<int>(n1, 0));
    Matrix M2(n2, vector<int>(n2, 0));
    GenerateMatrixAdjacency(M1, n1);
    GenerateMatrixAdjacency(M2, n2);
    vector<Graph> graphs = { {M1, AdjacencyList(M1), "G1"}, {M2, AdjacencyList(M2), "G2"} };
    PrintMatrix(graphs[0].M, "G1");
    PrintMatrix(graphs[1].M, "G2");
    PrintAdjacencyList(graphs[0].L, "G1");
    cout << endl;
    PrintAdjacencyList(graphs[1].L, "G2");
    cout << endl;

    int choice;


    do {
        cout << "1. Отождествление вершин" << endl;
        cout << "2. Стягивание рёбер" << endl;
        cout << "3. Расщепление вершин" << endl;
        cout << "4. Объединение G1 и G2" << endl;
        cout << "5. Пересечение G1 и G2" << endl;
        cout << "6. Кольцевая сумма G1 и G2" << endl;
        cout << "7. Декартово произведение G1 x G2" << endl;
        cout << "0. Выход" << endl;
        choice = ReadInt("Выберите действие: ", 0, 7);
        switch (choice) {
            case 1: {
                const Graph &G = ChooseGraph(graphs);
                int u = ReadVertex("Введите первую вершину: ", G.M.size());
                int v = ReadVertex("Введите вторую вершину: ", G.M.size());
                if (u != v) {
                    Matrix R = VertexIdentification(G.M, u, v, true);
                    PrintMatrix(R, G.name + " после отождествления");
                    AdjList L = VertexIdentificationList(G.L, u, v, true);
                    PrintAdjacencyList(L, G.name + " после отождествления");
                }
                else cout << "Вершины не должны быть одинаковыми" << endl;
                break;
            }
            case 2: {
                const Graph &G = ChooseGraph(graphs);
                int u = ReadVertex("Введите первую вершину: ", G.M.size());
                int v = ReadVertex("Введите вторую вершину: ", G.M.size());
                if (u != v && G.M[u][v] == 1) {
                    Matrix R = VertexIdentification(G.M, u, v, false);
                    PrintMatrix(R, G.name + " после стягивания ребра");
                    AdjList L = VertexIdentificationList(G.L, u, v, false);
                    PrintAdjacencyList(L, G.name + " после стягивания ребра");
                }
                else cout << "Вершины не должны быть одинаковыми / вершины должны быть смежными" << endl;
                break;
            }
            case 3: {
                const Graph &G = ChooseGraph(graphs);
                int u = ReadVertex("Введите вершину для расщепления: ", G.M.size());
                Matrix R = VertexSplitting(G.M, u);
                PrintMatrix(R, G.name + " после расщепления вершины");
                AdjList L = VertexSplittingList(G.L, u);
                PrintAdjacencyList(L, G.name + " после расщепления вершины");
                break;
            }
            case 4: {
                Matrix R = GraphUnion(graphs[0].M, graphs[1].M);
                PrintMatrix(R, "объединение G1 и G2");
                AdjList L = GraphUnionList(graphs[0].L, graphs[1].L);
                PrintAdjacencyList(L, "объединение G1 и G2");
                break;
            }
            case 5: {
                Matrix R = GraphIntersection(graphs[0].M, graphs[1].M);
                PrintMatrix(R, "пересечение G1 и G2");
                AdjList L = GraphIntersectionList(graphs[0].L, graphs[1].L);
                PrintAdjacencyList(L, "пересечение G1 и G2");
                break;
            }
            case 6: {
                vector<size_t> keep;
                Matrix R = GraphRingSum(graphs[0].M, graphs[1].M, keep);
                vector<size_t> listKeep;
                AdjList L = GraphRingSumList(graphs[0].L, graphs[1].L, listKeep);
                if (R.empty()) {
                    cout << "Кольцевая сумма не содержит рёбер, все вершины изолированы" << endl;
                } else {
                    PrintMatrix(R, "кольцевая сумма G1 и G2");
                    cout << "Соответствие номеров (новый: старый):";
                    for (size_t i = 0; i < keep.size(); i++) {
                        cout << " " << i << ":" << keep[i];
                    }
                    cout << endl;
                }
                if (L.empty()) {
                    cout << "Список кольцевой суммы не содержит рёбер, все вершины изолированы" << endl;
                } else {
                    PrintAdjacencyList(L, "кольцевая сумма G1 и G2");
                    cout << "Соответствие номеров в списке (новый: старый):";
                    for (size_t i = 0; i < listKeep.size(); i++) {
                        cout << " " << i << ":" << listKeep[i];
                    }
                    cout << endl;
                }
                break;
            }
            case 7: {
                Matrix R = CartesianProduct(graphs[0].M, graphs[1].M);
                PrintMatrix(R, "декартово произведение G1 x G2");
                cout << "Номер вершины = a * " << graphs[1].M.size() << " + b, где a из G1, b из G2" << endl;
                break;
            }
            case 0:
                break;
            default: cout << "Неверный пункт меню" << endl;
        }
    } while (choice != 0);
    return 0;
}
