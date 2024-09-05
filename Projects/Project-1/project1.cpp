#include <iostream>
using namespace std;

class SparseRow {
    protected:
        int row; 
        int col; 
        int value;  
    public:
        SparseRow (); 
        void display(); 
        friend ostream& operator<<(ostream& s, const SparseRow& sr); 
        void setRow(int r); 
        void setCol(int c); 
        void setValue(int v); 
        int getRow() const; 
        int getCol() const; 
        int getValue() const; 
};

SparseRow::SparseRow() {
    row = -1;
    col = -1;
    value = 0;
}

void SparseRow::display() {
    cout << "Row#: " << row << ", Column#: " << col << ", Value: " << value << endl;
}

ostream& operator<<(ostream& s, const SparseRow& sr) {
    s << "Row#: " << sr.getRow() << ", Column#: " << sr.getCol() << ", Value: " << sr.getValue(); 
    return s;
}

void SparseRow::setRow(int r) {
    row = r;
}

void SparseRow::setCol(int c) {
    col = c;
}

void SparseRow::setValue(int v) {
    value = v;
}

int SparseRow::getRow() const {
    return row;
}

int SparseRow::getCol() const {
    return col;
}

int SparseRow::getValue() const {
    return value;
}

class SparseMatrix {
    protected:
        int noRows; 
        int noCols; 
        int commonValue; 
        int noNonSparseValues;
        SparseRow* myMatrix;
    public:
        SparseMatrix ();
        SparseMatrix (int n, int m, int cv);
        SparseMatrix* Transpose ();
        SparseMatrix* Multiply (SparseMatrix& M);
        SparseMatrix* Add (SparseMatrix& M);
        friend ostream& operator<< (ostream& s, const SparseMatrix& sm);
        void displayMatrix(); 
        void setNoRows(int n);
        void setNoCols(int m);
        void setCommonValue(int cv);
        void setNoNonSparseValues(int nsv);
        int getNoRows(); 
        int getNoCols(); 
        int getCommonValue(); 
        int getNoNonSparseValues(); 
};

SparseMatrix::SparseMatrix() { 
    noRows = 0;
    noCols = 0;
    commonValue = 0;
    noNonSparseValues = 0;
    myMatrix = nullptr;
}

SparseMatrix::SparseMatrix(int n, int m, int cv) {
    noRows = n;
    noCols = m;
    commonValue = cv;
    myMatrix = nullptr;
}

SparseMatrix* SparseMatrix::Transpose() {
    SparseMatrix* transposedMatrix = new SparseMatrix(noCols, noRows, commonValue);
    transposedMatrix->noNonSparseValues = noNonSparseValues;
    transposedMatrix->myMatrix = new SparseRow[noNonSparseValues];

    int index = 0;
    for (int i = 0; i < noCols; i++) {
        for (int j = 0; j < noNonSparseValues; j++) {
            if (myMatrix[j].getCol() == i) {
                transposedMatrix->myMatrix[index].setRow(myMatrix[j].getCol());
                transposedMatrix->myMatrix[index].setCol(myMatrix[j].getRow());
                transposedMatrix->myMatrix[index].setValue(myMatrix[j].getValue());
                index++;
            }
        }
    }

    return transposedMatrix;
}

SparseMatrix* SparseMatrix::Multiply(SparseMatrix& M) {
    if (noCols != M.noRows) {
        cout << "Error: Incompatible matrix dimensions for multiplication." << endl;
        return nullptr;
    }

    SparseMatrix* multipliedMatrix = new SparseMatrix(noRows, M.noCols, commonValue);
    multipliedMatrix->noNonSparseValues = 0;
    multipliedMatrix->myMatrix = new SparseRow[noRows * M.noCols];

    for (int i = 0; i < noRows; i++) {
        for (int j = 0; j < M.noCols; j++) {
            int sum = 0;
            for (int k = 0; k < noCols; k++) {
                int value1 = myMatrix[i].getValue(); 
                int value2 = M.myMatrix[k * M.noCols + j].getValue();
                sum += value1 * value2;
            }
            if (sum != commonValue) {
                multipliedMatrix->myMatrix[multipliedMatrix->noNonSparseValues].setRow(i);
                multipliedMatrix->myMatrix[multipliedMatrix->noNonSparseValues].setCol(j);
                multipliedMatrix->myMatrix[multipliedMatrix->noNonSparseValues].setValue(sum);
                multipliedMatrix->noNonSparseValues++;
            }
        }
    }

    return multipliedMatrix;
}

SparseMatrix* SparseMatrix::Add(SparseMatrix& M) {
    if (noRows != M.noRows || noCols != M.noCols) {
        cout << "Error: Incompatible matrix dimensions for addition." << endl;
        return nullptr;
    }

    SparseMatrix* addedMatrix = new SparseMatrix(noRows, noCols, commonValue);
    addedMatrix->noNonSparseValues = 0;
    addedMatrix->myMatrix = new SparseRow[noNonSparseValues + M.noNonSparseValues];

    int i = 0, j = 0, index = 0;
    while (i < noNonSparseValues && j < M.noNonSparseValues) {
        int row1 = myMatrix[i].getRow();
        int col1 = myMatrix[i].getCol();
        int row2 = M.myMatrix[j].getRow();
        int col2 = M.myMatrix[j].getCol();

        if (row1 < row2 || (row1 == row2 && col1 < col2)) {
            addedMatrix->myMatrix[index].setRow(row1);
            addedMatrix->myMatrix[index].setCol(col1);
            addedMatrix->myMatrix[index].setValue(myMatrix[i].getValue());
            i++;
        } else if (row1 > row2 || (row1 == row2 && col1 > col2)) {
            addedMatrix->myMatrix[index].setRow(row2);
            addedMatrix->myMatrix[index].setCol(col2);
            addedMatrix->myMatrix[index].setValue(M.myMatrix[j].getValue());
            j++;
        } else {
            int sum = myMatrix[i].getValue() + M.myMatrix[j].getValue();
            if (sum != commonValue) {
                addedMatrix->myMatrix[index].setRow(row1);
                addedMatrix->myMatrix[index].setCol(col1);
                addedMatrix->myMatrix[index].setValue(sum);
                addedMatrix->noNonSparseValues++;
            }
            i++;
            j++;
        }
        index++;
    }

    while (i < noNonSparseValues) {
        addedMatrix->myMatrix[index].setRow(myMatrix[i].getRow());
        addedMatrix->myMatrix[index].setCol(myMatrix[i].getCol());
        addedMatrix->myMatrix[index].setValue(myMatrix[i].getValue());
        i++;
        index++;
    }

    while (j < M.noNonSparseValues) {
        addedMatrix->myMatrix[index].setRow(M.myMatrix[j].getRow());
        addedMatrix->myMatrix[index].setCol(M.myMatrix[j].getCol());
        addedMatrix->myMatrix[index].setValue(M.myMatrix[j].getValue());
        j++;
        index++;
    }

    addedMatrix->noNonSparseValues = index;

    return addedMatrix;
}

ostream& operator<<(ostream& s, const SparseMatrix& sm) {
    for (int i = 0; i < sm.noNonSparseValues; i++) {
        s << sm.myMatrix[i] << endl;
    }
    return s;
}

void SparseMatrix::displayMatrix() {
    int index = 0;
    for (int i = 0; i < noRows; i++) {
        for (int j = 0; j < noCols; j++) {
            if (index < noNonSparseValues && myMatrix[index].getRow() == i && myMatrix[index].getCol() == j) {
                cout << myMatrix[index].getValue() << " ";
                index++;
            } else {
                cout << commonValue << " ";
            }
        }
        cout << endl;
    }
}

void SparseMatrix::setNoRows(int n) {
    noRows = n;
}

void SparseMatrix::setNoCols(int m) {
    noCols = m;
}

void SparseMatrix::setCommonValue(int cv) {
    commonValue = cv;
}

void SparseMatrix::setNoNonSparseValues(int nsv) {
    noNonSparseValues = nsv;
}

int SparseMatrix::getNoRows() {
    return noRows;
}

int SparseMatrix::getNoCols() {
    return noCols;
}

int SparseMatrix::getCommonValue() {
    return commonValue;
}

int SparseMatrix::getNoNonSparseValues() {
    return noNonSparseValues;
}

int main () {
    int n, m, cv, noNSV;
    SparseMatrix* temp;

    cin >> n >> m >> cv >> noNSV;
    SparseMatrix* firstOne = new SparseMatrix(n, m, cv);

    cin >> n >> m >> cv >> noNSV;
    SparseMatrix* secondOne = new SparseMatrix(n, m, cv);

    cout << "First one in matrix format" << endl;
    firstOne->displayMatrix();

    cout << "First one in sparse matrix format" << endl;
    cout << *firstOne;

    cout << "Second one in matrix format" << endl;
    secondOne->displayMatrix();

    cout << "Second one in sparse matrix format" << endl;
    cout << *secondOne;

    cout << "Transpose of the first one in matrix" << endl;
    cout << *firstOne->Transpose();

    cout << "Matrix Addition Result" << endl;
    temp = firstOne->Add(*secondOne);
    cout << temp;
    temp->displayMatrix();

    cout << "Matrix Multiplication Result" << endl;
    temp = firstOne->Multiply(*secondOne);
    cout << temp;
    temp->displayMatrix();
}
