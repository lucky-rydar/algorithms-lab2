#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include <optional>

using namespace std;

template <typename Key, typename Value>
class AVL
{
    public:
        class node
        {
            public:
                Key key;
                Value value;

                int height;
                node * left;
                node * right;
                node(Key k, Value v){
                    height = 1;
                    key = k;
                    value = v;
                    left = NULL;
                    right = NULL;
                }
        };

        struct key_value
        {
            Key key;
            Value val;
        };


        node * root = NULL;
        int n;
        
        void insert(Key x, Value val){
            root=insertUtil(root, x, val);
        }
        
        void remove(Key x){
            root=removeUtil(root, x);
        }
        
        node * search(Key x){
            return searchUtil(root,x);
        }
        
        void inorder(){
            inorderUtil(root);
            cout<<endl;
        }
        
        vector<key_value> getList() {
            vector<key_value> ret;
            getListUtil(ret, root);
            return ret;
        }
        
    private:
        void getListUtil(vector<key_value>& ret, node* head) {
            if(head==NULL) return ;
            getListUtil(ret, head->left);
            ret.push_back({head->key, head->value});
            getListUtil(ret, head->right);
        }

        int height(node * head){
            if(head==NULL) return 0;
            return head->height;
        }
        node * rightRotation(node * head){
            node * newhead = head->left;
            head->left = newhead->right;
            newhead->right = head;
            head->height = 1+max(height(head->left), height(head->right));
            newhead->height = 1+max(height(newhead->left), height(newhead->right));
            return newhead;
        }

        node * leftRotation(node * head){
            node * newhead = head->right;
            head->right = newhead->left;
            newhead->left = head;
            head->height = 1+max(height(head->left), height(head->right));
            newhead->height = 1+max(height(newhead->left), height(newhead->right));
            return newhead;
        }

        void inorderUtil(node * head){
            if(head==NULL) return ;
            inorderUtil(head->left);
            cout<<head->value<<" ";
            inorderUtil(head->right);
        }

        node * insertUtil(node * head, Key x, Value val){
            if(head==NULL){
                n+=1;
                node * temp = new node(x, val);
                return temp;
            }
            if(x < head->key) head->left = insertUtil(head->left, x, val);
            else if(x > head->key) head->right = insertUtil(head->right, x, val);
            head->height = 1 + max(height(head->left), height(head->right));
            int bal = height(head->left) - height(head->right);
            if(bal>1){
                if(x < head->left->key){
                    return rightRotation(head);
                }else{
                    head->left = leftRotation(head->left);
                    return rightRotation(head);
                }
            }else if(bal<-1){
                if(x > head->right->key){
                    return leftRotation(head);
                }else{
                    head->right = rightRotation(head->right);
                    return leftRotation(head);
                }
            }
            return head;
        }
        node * removeUtil(node * head, Key x){
            if(head==NULL) return NULL;
            if(x < head->key){
                head->left = removeUtil(head->left, x);
            }else if(x > head->key){
                head->right = removeUtil(head->right, x);
            }else{
                node * r = head->right;
                if(head->right==NULL){
                    node * l = head->left;
                    delete(head);
                    head = l;
                }else if(head->left==NULL){
                    delete(head);
                    head = r;
                }else{
                    while(r->left!=NULL) r = r->left;
                    head->key = r->key;
                    head->right = removeUtil(head->right, r->key);
                }
            }
            if(head==NULL) return head;
            head->height = 1 + max(height(head->left), height(head->right));
            int bal = height(head->left) - height(head->right);
            if(bal>1){
                if(height(head->left) >= height(head->right)){
                    return rightRotation(head);
                }else{
                    head->left = leftRotation(head->left);
                    return rightRotation(head);
                }
            }else if(bal < -1){
                if(height(head->right) >= height(head->left)){
                    return leftRotation(head);
                }else{
                    head->right = rightRotation(head->right);
                    return leftRotation(head);
                }
            }
            return head;
        }
        node * searchUtil(node * head, Key x){
            if(head == NULL) return NULL;
            Key k = head->key;
            if(k == x) return head;
            if(k > x) return searchUtil(head->left, x);
            if(k < x) return searchUtil(head->right, x);

            return nullptr;
        }
};


class DBConfig
{
public:
    DBConfig() {}

    DBConfig(size_t num_of_cols, string filename = "db.dsql"){
        this->filename = filename;
        this->num_of_cols = num_of_cols;
    }

    DBConfig(const DBConfig& other) {
        filename = other.filename;
        num_of_cols = other.num_of_cols;
        column_names = other.column_names;
    }

    void operator=(const DBConfig& other) {
        filename = other.filename;
        num_of_cols = other.num_of_cols;
    }

    void setColumns(vector<string> columns) {
        column_names = columns;
    }

    string filename;
    size_t num_of_cols;
    vector<string> column_names;
};

using Raw = vector<string>;
const string delim = "|";

string rawToString(Raw raw) {
    string ret = "";
    
    for(size_t i = 0; i < raw.size()-1; i++) {
        ret += raw[i] + delim;
    }
    ret += raw[raw.size() - 1];

    return ret;
}

class DB
{
public:
    DB(DBConfig cfg) : config(cfg) {

    }

    Raw& get(size_t key) {
        return tree.search(key)->value;
    }

    void set(size_t key, Raw value) {
        auto searched = tree.search(key);
        if(searched == nullptr) {
            tree.insert(key, value);
        } else {
            searched->value = value;
        }
    }

    void remove(size_t key) {
        tree.remove(key);
    }

    void save() {
        file.open(config.filename, fstream::trunc | fstream::out);
        auto list = tree.getList();
        for(const auto& item : list) {
            string data = "";
            data += to_string(item.key) + delim + rawToString(item.val) + "\n";
            file.write(data.c_str(), data.length());
        }
        file.close();
    }

    void load() {
        // load from file element by element parse and add to tree
        file.open(config.filename, fstream::in);
        if(!file.is_open()) {
            file.open(config.filename, fstream::out);
            file.close();
            return;
        }

        while(!file.eof()) {
            string buff;
            getline(file, buff);
            auto parsed = DB::parseLine(buff);
            
            if(parsed.has_value()){
                set(parsed.value().first, parsed.value().second);
            }
        }
        file.close();
    }
private:
    static std::optional<std::pair<size_t, Raw>> parseLine(string input) {
        if(input == "") {
            return std::optional<std::pair<size_t, Raw>>();
        }
        
        regex delimiterRegex("\\" + delim);
        vector<string> res;

        copy(sregex_token_iterator(input.begin(), input.end(), delimiterRegex, -1),
            sregex_token_iterator(),
            back_inserter(res));

        if(res.size() == 0) {
            return std::optional<std::pair<size_t, Raw>>();
        }

        return std::pair<size_t, Raw>(std::stoull(res[0]), std::vector<string>(res.begin() + 1, res.end()));
    }

    DBConfig config;
    fstream file;
    AVL<size_t, Raw> tree;

};

int main(){
    
    /*AVL<size_t, string> t;
    t.insert(1, "one");
    t.insert(2, "two");
    t.insert(3, "three");
    t.insert(4, "four");
    t.insert(5, "five");
    t.insert(6, "six");
    t.insert(7, "seven");
    
    t.remove(5);
    t.remove(6);
    t.remove(7);
    
    auto r = t.getList();
    */

    try {
        DBConfig cfg(2);
        DB db(cfg);
        db.load();
        
        /*auto res = db.get(1);

        for(auto item : res) cout << item << " ";
        cout << endl;*/
        
        //db.set(1, {"one", "first"});
        db.remove(1);
        //db.set(2, {"two", "second"});
        db.set(3, {"three", "third"});
        cout << db.get(2)[0] << endl; 
        db.save();
            
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

