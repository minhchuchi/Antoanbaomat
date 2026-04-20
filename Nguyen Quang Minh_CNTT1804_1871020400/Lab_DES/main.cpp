#include <iostream>
#include <string>
#include <vector>
#include <bitset>

using namespace std;

// ==========================================
// 1. CÁC HÀM TRỢ GIÚP
// ==========================================
string convert_decimal_to_binary(int decimal) {
    return bitset<4>(decimal).to_string();
}

int convert_binary_to_decimal(const string& binary) {
    return stoi(binary, nullptr, 2);
}

string Xor(const string& a, const string& b) {
    string result = "";
    for (size_t i = 0; i < b.size(); i++) {
        result += (a[i] != b[i]) ? '1' : '0';
    }
    return result;
}

// Bảng hoán vị khởi tạo (IP)
string initial_permutation(const string& input) {
    const int initial_permutation[64] = {
        58,50,42,34,26,18,10,2, 60,52,44,36,28,20,12,4,
        62,54,46,38,30,22,14,6, 64,56,48,40,32,24,16,8,
        57,49,41,33,25,17,9,1,  59,51,43,35,27,19,11,3,
        61,53,45,37,29,21,13,5, 63,55,47,39,31,23,15,7
    };
    string permuted = "";
    for (int i = 0; i < 64; i++) {
        permuted += input[initial_permutation[i] - 1];
    }
    return permuted;
}

// Bảng hoán vị nghịch đảo (IP^-1)
string inverse_initial_permutation(const string& input) {
    const int inverse_permutation[64] = {
        40,8,48,16,56,24,64,32, 39,7,47,15,55,23,63,31,
        38,6,46,14,54,22,62,30, 37,5,45,13,53,21,61,29,
        36,4,44,12,52,20,60,28, 35,3,43,11,51,19,59,27,
        34,2,42,10,50,18,58,26, 33,1,41,9,49,17,57,25
    };
    string permuted = "";
    for (int i = 0; i < 64; i++) {
        permuted += input[inverse_permutation[i] - 1];
    }
    return permuted;
}

// Hàm xử lý Padding: Tự động đắp thêm số '0' cho đủ bội số của 64 bit (Giải Q2)
string pad_to_64bits(string bin_str) {
    if (bin_str.empty()) return "0000000000000000000000000000000000000000000000000000000000000000";
    while (bin_str.length() % 64 != 0) {
        bin_str += "0"; 
    }
    return bin_str;
}

// ==========================================
// 2. LỚP SINH KHÓA (KEY GENERATOR)
// ==========================================
class KeyGenerator {
private:
    string key;
    vector<string> roundKeys;
    
    int pc1[56] = {
        57,49,41,33,25,17,9,
        1,58,50,42,34,26,18,
        10,2,59,51,43,35,27,
        19,11,3,60,52,44,36,
        63,55,47,39,31,23,15,
        7,62,54,46,38,30,22,
        14,6,61,53,45,37,29,
        21,13,5,28,20,12,4
    };
    
    int pc2[48] = {
        14,17,11,24,1,5,
        3,28,15,6,21,10,
        23,19,12,4,26,8,
        16,7,27,20,13,2,
        41,52,31,37,47,55,
        30,40,51,45,33,48,
        44,49,39,56,34,53,
        46,42,50,36,29,32
    };

    string shift_left_once(const string& key_chunk) {
        return key_chunk.substr(1) + key_chunk[0];
    }
    string shift_left_twice(string key_chunk) {
        return shift_left_once(shift_left_once(key_chunk));
    }

public:
    KeyGenerator(const string& input_key) : key(input_key) {}
    
    void generateRoundKeys() {
        roundKeys.clear();
        string permutedKey = "";
        for (int i = 0; i < 56; ++i) { permutedKey += key[pc1[i] - 1]; }
        
        string left = permutedKey.substr(0, 28);
        string right = permutedKey.substr(28, 28);
        
        for (int i = 0; i < 16; ++i) {
            if (i == 0 || i == 1 || i == 8 || i == 15) {
                left = shift_left_once(left);
                right = shift_left_once(right);
            } else {
                left = shift_left_twice(left);
                right = shift_left_twice(right);
            }
            string combinedKey = left + right;
            string roundKey = "";
            for (int j = 0; j < 48; ++j) { roundKey += combinedKey[pc2[j] - 1]; }
            roundKeys.push_back(roundKey);
        }
    }
    const vector<string>& getRoundKeys() const { return roundKeys; }
};

// ==========================================
// 3. LỚP DES (MÃ HÓA & GIẢI MÃ)
// ==========================================
class DES {
private:
    vector<string> round_keys;
    
    int expansion_table[48] = {
        32,1,2,3,4,5,4,5,
        6,7,8,9,8,9,10,11,
        12,13,12,13,14,15,16,17,
        16,17,18,19,20,21,20,21,
        22,23,24,25,24,25,26,27,
        28,29,28,29,30,31,32,1
    };
    
    int permutation_tab[32] = {
        16,7,20,21,29,12,28,17,
        1,15,23,26,5,18,31,10,
        2,8,24,14,32,27,3,9,
        19,13,30,6,22,11,4,25
    };
    
    int substitution_boxes[8][4][16] = {
        {
            {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
            {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
            {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
            {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
        },
        {
            {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
            {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
            {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
            {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
        },
        {
            {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
            {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
            {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
            {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
        },
        {
            {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
            {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
            {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
            {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
        },
        {
            {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
            {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
            {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
            {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
        },
        {
            {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
            {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
            {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
            {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
        },
        {
            {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
            {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
            {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
            {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
        },
        {
            {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
            {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
            {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
            {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
        }
    };

public:
    DES(const vector<string>& keys) : round_keys(keys) {}

    // HÀM MÃ HÓA
    string encrypt(const string& input) {
        string perm = initial_permutation(input);
        string left = perm.substr(0, 32);
        string right = perm.substr(32, 32);
        
        for (int i = 0; i < 16; i++) {
            string right_expanded = "";
            for (int j = 0; j < 48; j++) { right_expanded += right[expansion_table[j] - 1]; }
            
            string xored = Xor(round_keys[i], right_expanded);
            string res = "";
            
            for (int j = 0; j < 8; j++) {
                string row1 = xored.substr(j * 6, 1) + xored.substr(j * 6 + 5, 1);
                int row = convert_binary_to_decimal(row1);
                string col1 = xored.substr(j * 6 + 1, 4);
                int col = convert_binary_to_decimal(col1);
                int val = substitution_boxes[j][row][col];
                res += convert_decimal_to_binary(val);
            }
            
            string perm2 = "";
            for (int j = 0; j < 32; j++) { perm2 += res[permutation_tab[j] - 1]; }
            
            string new_right = left;
            left = Xor(perm2, right);
            right = new_right;
        }
        return inverse_initial_permutation(right + left);
    }

    // HÀM GIẢI MÃ (Giải Q3)
    string decrypt(const string& input) {
        string perm = initial_permutation(input);
        string left = perm.substr(0, 32);
        string right = perm.substr(32, 32);
        
        // Sử dụng khóa vòng ngược lại (từ 15 về 0)
        for (int i = 15; i >= 0; i--) {
            string right_expanded = "";
            for (int j = 0; j < 48; j++) { right_expanded += right[expansion_table[j] - 1]; }
            
            string xored = Xor(round_keys[i], right_expanded);
            string res = "";
            
            for (int j = 0; j < 8; j++) {
                string row1 = xored.substr(j * 6, 1) + xored.substr(j * 6 + 5, 1);
                int row = convert_binary_to_decimal(row1);
                string col1 = xored.substr(j * 6 + 1, 4);
                int col = convert_binary_to_decimal(col1);
                int val = substitution_boxes[j][row][col];
                res += convert_decimal_to_binary(val);
            }
            
            string perm2 = "";
            for (int j = 0; j < 32; j++) { perm2 += res[permutation_tab[j] - 1]; }
            
            string new_right = left;
            left = Xor(perm2, right);
            right = new_right;
        }
        return inverse_initial_permutation(right + left);
    }
};

// ==========================================
// 4. HÀM CHÍNH (MAIN)
// ==========================================
int main() {
    cout << "--- HE THONG MA HOA DES & TRIPLE DES ---\n";
    
    // Yêu cầu nhập liệu (CHUỖI NHỊ PHÂN 0 VÀ 1)
    string plaintext, key1, key2, key3;
    cout << "Nhap ban ro (dang nhi phan 0 va 1): ";
    cin >> plaintext;
    cout << "Nhap khoa K1: ";
    cin >> key1;
    cout << "Nhap khoa K2 (Cho 3DES): ";
    cin >> key2;
    cout << "Nhap khoa K3 (Cho 3DES): ";
    cin >> key3;

    // Padding cho đủ khối 64 bit (Giải Q2)
    plaintext = pad_to_64bits(plaintext);
    key1 = pad_to_64bits(key1).substr(0, 64);
    key2 = pad_to_64bits(key2).substr(0, 64);
    key3 = pad_to_64bits(key3).substr(0, 64);

    cout << "\n[!] Ban ro sau khi Padding:\n" << plaintext << "\n\n";

    // Khởi tạo các KeyGenerator
    KeyGenerator kg1(key1); kg1.generateRoundKeys();
    KeyGenerator kg2(key2); kg2.generateRoundKeys();
    KeyGenerator kg3(key3); kg3.generateRoundKeys();

    DES des1(kg1.getRoundKeys());
    DES des2(kg2.getRoundKeys());
    DES des3(kg3.getRoundKeys());

    string full_ciphertext = "";
    string full_decrypted = "";
    string triple_des_cipher = "";
    string triple_des_decrypted = "";

    // Xử lý chia khối 64 bit (Giải Q2)
    for (size_t i = 0; i < plaintext.length(); i += 64) {
        string block = plaintext.substr(i, 64);

        // --- KIỂM THỬ DES THƯỜNG ---
        string c_block = des1.encrypt(block);
        full_ciphertext += c_block;
        full_decrypted += des1.decrypt(c_block);

        // --- KIỂM THỬ TRIPLE DES (Giải Q4) ---
        // Chiều Mã Hóa: E(K1) -> D(K2) -> E(K3)
        string stage1 = des1.encrypt(block);
        string stage2 = des2.decrypt(stage1);
        string stage3 = des3.encrypt(stage2);
        triple_des_cipher += stage3;
        
        // Chiều Giải Mã 3DES: D(K3) -> E(K2) -> D(K1)
        string d_stage1 = des3.decrypt(stage3);
        string d_stage2 = des2.encrypt(d_stage1);
        string d_stage3 = des1.decrypt(d_stage2);
        triple_des_decrypted += d_stage3;
    }

    cout << "--- KET QUA DES THUONG ---" << endl;
    cout << "Ciphertext : " << full_ciphertext << endl;
    cout << "Decrypted  : " << full_decrypted << endl;

    cout << "\n--- KET QUA TRIPLE DES ---" << endl;
    cout << "3DES Cipher: " << triple_des_cipher << endl;
    cout << "3DES Decrypt: " << triple_des_decrypted << endl;

    return 0;
}