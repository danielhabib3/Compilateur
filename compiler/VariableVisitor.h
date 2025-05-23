#pragma once
#include "generated/ifccBaseVisitor.h"
#include "generated/ifccParser.h"
#include <map>
#include <string>
#include <list>

using namespace std;

enum Types { INT, CHAR };

enum ErrorType { WARNING, ERROR };

const string Etiquettes[] = { "INT", "CHAR" };

typedef struct Variable {
    int location;
    int type;
    int line;
    int column;
    int nbUse;
    int set;  // que pour les variables
    int size;  // que pour les tableaux
    bool isArray; // true si c'est un tableau, false sinon
    // Constructor
    Variable()
    {
        isArray = false;
    }
} infosVariable;

class Block {

    public:

        Block(Block* parent = nullptr) {
            this->parent = parent;
            this->_variables = {};
        }

        // Destructor
        ~Block() {
            for (auto child : children) {
                delete child;
            }
        }

        // Getter for variables
        std::map<std::string, infosVariable> getVariables() const {
            return _variables;
        }

        // Setter for variables
        void setVariables(const std::map<std::string, infosVariable>& variables) {
            _variables = variables;
        }

        // Method to add a child block
        void addChild(Block* child) {
            children.push_back(child);
            notVisitedChildren.push_back(child);
        }

        // Method to generate a CFG representation for blocks
        void affiche_bloc(std::ostream &o, string functionName) {
            o << "    subgraph G_" + functionName + " {\n";
            o << "        node [shape=ellipse];\n\n";
            affiche_bloc_recursive(o);
            o << "    }\n";
        }

    private:

        int affiche_bloc_recursive(std::ostream &o, int currentNode = 0) {

            int childNode = currentNode + 1;
            for (auto child : children) {
                o << "        Node" << currentNode << " -> Node" << childNode << ";\n";
                int next_id = child->affiche_bloc_recursive(o, childNode);
                childNode = next_id;
            }
            return childNode;
        }

    public:
        Block * parent;
        std::vector<Block *> children;
        std::map<std::string, infosVariable> _variables;
        std::vector<Block *> notVisitedChildren;
};



class VariableVisitor : public ifccBaseVisitor {
    public:
        // constructor
        VariableVisitor() {
            _variableErrorsWarnings = {};
            current_rootBlock = nullptr;
            currentBlock = nullptr;
        };


        map<string, ErrorType> getVariableErrorsWarnings() {
            return _variableErrorsWarnings;
        }

        void setRootBlocks(vector<Block*> rootBlocks) {
            _rootBlocks = rootBlocks;
        }

        vector<Block*> getRootBlocks() {
            return _rootBlocks;
        }
        
        void setVariableErrorsWarnings(map<string, ErrorType> variableErrors) {
            _variableErrorsWarnings = variableErrors;
        }

        int getNextFreeLocation() const {
            return next_free_location;
        }

        void setNextFreeLocation(int location) {
            next_free_location = location;
        }

        int countErrors() {
            int count = 0;
            for(auto it = _variableErrorsWarnings.begin(); it != _variableErrorsWarnings.end(); it++)
            {
                if(it->second == ERROR)
                {
                    count++;
                }
            }
            return count;
        }

        virtual antlrcpp::Any visitBlock(ifccParser::BlockContext *ctx) override;
        virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext *ctx) override;
        virtual antlrcpp::Any visitExprAffectation(ifccParser::ExprAffectationContext *ctx) override;
        virtual antlrcpp::Any visitExprID(ifccParser::ExprIDContext *ctx) override;
        virtual antlrcpp::Any visitAffectationDeclaration(ifccParser::AffectationDeclarationContext *ctx) override;
        virtual antlrcpp::Any visitDeclarationTable(ifccParser::DeclarationTableContext *ctx) override ;
        virtual antlrcpp::Any visitAffectationDeclarationTable(ifccParser::AffectationDeclarationTableContext *ctx) override ;
        virtual antlrcpp::Any visitExprTable(ifccParser::ExprTableContext *ctx) override;
        virtual antlrcpp::Any visitExprAffectationTable(ifccParser::ExprAffectationTableContext *ctx) override;
        virtual antlrcpp::Any visitFunction_definition(ifccParser::Function_definitionContext *ctx) override;
    
        private:
            // map<string, infosVariable> _variables;
            Block* currentBlock;
            Block* current_rootBlock;
            vector<Block*> _rootBlocks;
            map<string, ErrorType> _variableErrorsWarnings;
            int next_free_location;
        
};