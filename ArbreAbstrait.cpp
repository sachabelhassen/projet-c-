#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"
#include <vector>
#include <typeinfo>
using namespace std;
////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
    for (unsigned int i = 0; i < m_instructions.size(); i++)
        m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
    if (instruction != nullptr) m_instructions.push_back(instruction);
}

void NoeudSeqInst::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    for (auto instr : m_instructions) {
        instr->traduitEnCPP(cout, indentation);
        if ((typeid (*instr) == typeid (NoeudAffectation*))
                || (typeid (*instr) == typeid (NoeudInstRepeter*))
                || (typeid (*instr) == typeid (NoeudInstEcrire*))
                || (typeid (*instr) == typeid (NoeudInstLire*))) {
            cout << ";";
        }
        cout << endl;
    }


}



////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
    int valeur = m_expression->executer(); // On exécute (évalue) l'expression
    ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudAffectation::traduitEnCPP(std::ostream& cout, unsigned int indentation) const {
    cout << setw(4 * indentation) << "";
    m_variable->traduitEnCPP(cout, 0);
    cout << "=";
    m_expression->traduitEnCPP(cout, 0);
}


////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
    int og, od, valeur;
    if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
    if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
    // Et on combine les deux opérandes en fonctions de l'opérateur
    if (this->m_operateur == "+") valeur = (og + od);
    else if (this->m_operateur == "-") valeur = (og - od);
    else if (this->m_operateur == "*") valeur = (og * od);
    else if (this->m_operateur == "==") valeur = (og == od);
    else if (this->m_operateur == "!=") valeur = (og != od);
    else if (this->m_operateur == "<") valeur = (og < od);
    else if (this->m_operateur == ">") valeur = (og > od);
    else if (this->m_operateur == "<=") valeur = (og <= od);
    else if (this->m_operateur == ">=") valeur = (og >= od);
    else if (this->m_operateur == "et") valeur = (og && od);
    else if (this->m_operateur == "ou") valeur = (og || od);
    else if (this->m_operateur == "non") valeur = (!og);
    else if (this->m_operateur == "/") {
        if (od == 0) throw DivParZeroException();
        valeur = og / od;
    }
    return valeur; // On retourne la valeur calculée
}

void NoeudOperateurBinaire::traduitEnCPP(std::ostream& cout, unsigned int indentation) const {
    cout << setw(4 * indentation) << "";
    m_operandeGauche->traduitEnCPP(cout, 0);
    cout << " " << m_operateur.getChaine() << " ";
    m_operandeDroit->traduitEnCPP(cout, 0);
}


////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstSi::executer() {
    if (m_condition->executer()) m_sequence->executer();
    return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudTantQue
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
    while (m_condition->executer()) m_sequence->executer();
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstTantQue::traduitEnCPP(std::ostream& cout, unsigned int indentation) const {
    cout << setw(4 * indentation) << "while(";
    m_condition->traduitEnCPP(cout, 0);
    cout << "){" << endl;
    m_sequence->traduitEnCPP(cout, indentation + 1);
    cout << setw(4 * indentation) << "}";
}


////////////////////////////////////////////////////////////////////////////////
// NoeudRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* sequence, Noeud* condition)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstRepeter::executer() {
    do {
        m_sequence->executer();
    } while (!(m_condition->executer()));
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstRepeter::traduitEnCPP(std::ostream& cout, unsigned int indentation) const {
    cout << setw(4 * indentation) << "" << "do " << endl;
    m_sequence->traduitEnCPP(cout, indentation + 1);
    cout << setw(4 * indentation) << "" << "while (";
    m_condition->traduitEnCPP(cout, 0);
    cout << ");";
}

////////////////////////////////////////////////////////////////////////////////
// NoeudPour
////////////////////////////////////////////////////////////////////////////////

NoeudInstPour::NoeudInstPour(Noeud* affecter, Noeud* condition, Noeud* action, Noeud* sequence)
: m_affecter(affecter), m_condition(condition), m_action(action), m_sequence(sequence) {
}

int NoeudInstPour::executer() {
    /*if(m_affecter){
        if(m_action){
            for (m_affecter->executer();m_condition->executer();m_action->executer()){
                m_sequence->executer();
            }
        }
        else {for()
     }
    if(m_action== NoeudAffectation){}*/


    for ((m_affecter == nullptr ? 0 : m_affecter->executer());
            m_condition->executer();
            (m_action == nullptr ? 0 : m_action->executer())) {
        m_sequence->executer();
    }
    return 0; // La valeur renvoyée ne représente rien !

}

void NoeudInstPour::traduitEnCPP(std::ostream& cout, unsigned int indentation) const {
    cout << setw(4 * indentation) << "" << "for (";
    m_affecter->traduitEnCPP(cout, 0);
    cout << "; ";
    m_condition->traduitEnCPP(cout, 0);
    cout << "; ";
    m_action->traduitEnCPP(cout, 0);
    cout << ") {" << endl;
    m_sequence->traduitEnCPP(cout, indentation + 1);
    cout << setw(4 * indentation) << "" << "}";
}


////////////////////////////////////////////////////////////////////////////////
// NoeudEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrire::NoeudInstEcrire()
: m_vecteurEcrire() {

}

void NoeudInstEcrire::ajoute(Noeud* parametre) {
    m_vecteurEcrire.push_back(parametre);
}

int NoeudInstEcrire::executer() {
    for (auto param : m_vecteurEcrire) {
        if ((typeid (*param) == typeid (SymboleValue)) && *((SymboleValue*) param) == "<CHAINE>") {
            string chaine = ((SymboleValue*) param)->getChaine();
            cout << chaine.substr(1, chaine.size() - 2);
        } else {
            cout << param->executer();
        }
    }
    return 0;
}

void NoeudInstEcrire::traduitEnCPP(std::ostream& cout, unsigned int indentation) const {
    cout << setw(4 * indentation) << "" << "cout";
    for (auto p : m_vecteurEcrire) {
        if ((typeid (*p) == typeid (SymboleValue) && *((SymboleValue*) p) == "<CHAINE>")) {
            cout << " << " << ((SymboleValue*) p)->getChaine();
        } else {
            cout << " << ";
            p->traduitEnCPP(cout, 0);
        }
    }
}




////////////////////////////////////////////////////////////////////////////////
// NoeudSiRiche
////////////////////////////////////////////////////////////////////////////////

NoeudInstSiRiche::NoeudInstSiRiche() : m_vecteurConditonInstruction() {

}

void NoeudInstSiRiche::ajouter(Noeud* instruction) {
    m_vecteurConditonInstruction.push_back(instruction);
}

int NoeudInstSiRiche::executer() {
    int nb = m_vecteurConditonInstruction.size();
    int fin = (nb % 2) ? (nb - 2) : (nb - 1);
    int i = 0;
    // on recherche la premiere condition vraie
    while (i < fin && !m_vecteurConditonInstruction[i]->executer())
        i += 2;
    if (i < fin)
        m_vecteurConditonInstruction[i + 1]->executer();
    else if (nb % 2)
        m_vecteurConditonInstruction[nb - 1]->executer();

    return 0;
}

void NoeudInstSiRiche::traduitEnCPP(std::ostream& cout, unsigned int indentation) const {
    int i = 0;
    cout << setw(4 * indentation) << "" << "if(";
    m_vecteurConditonInstruction[i]->traduitEnCPP(cout, 0);
    i++;
    cout << setw(4 * indentation) << "){"<< endl;
    m_vecteurConditonInstruction[i]->traduitEnCPP(cout, indentation + 1);
    cout << setw(4 * indentation) << "" << "}" << endl;
    i++;
    while (i <= m_vecteurConditonInstruction.size()-2) {
        cout << setw(4 * indentation) << "" << "else if (";
        m_vecteurConditonInstruction[i]->traduitEnCPP(cout, 0);
        i++;
        cout << setw(4 * indentation) << "){"<<endl;
        m_vecteurConditonInstruction[i]->traduitEnCPP(cout, indentation + 1);
        cout << setw(4 * indentation) << "" << "}" << endl;
        i++;
    }
    if (m_vecteurConditonInstruction.size() % 2) {
        cout << setw(4 * indentation) << "" << "else {" << endl;
        m_vecteurConditonInstruction[m_vecteurConditonInstruction.size() - 1]->traduitEnCPP(cout, indentation + 1);
        cout << setw(4 * indentation) << "" << "}";
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudLire
////////////////////////////////////////////////////////////////////////////////

NoeudInstLire::NoeudInstLire() {
}

void NoeudInstLire::ajoute(Noeud* variable) {
    m_varLire.push_back(variable);
}

int NoeudInstLire::executer() {
    for (auto var : m_varLire) {
        int val;
        cin >> val;
        ((SymboleValue*) var)->setValeur(val);
    }
    return 0;
}

void NoeudInstLire::traduitEnCPP(std::ostream& cout, unsigned int indentation) const {
    cout << setw(4 * indentation) << "" << "cin";
    for (auto p : m_varLire) {
        cout << " >> ";
        p->traduitEnCPP(cout, 0);
    }
}