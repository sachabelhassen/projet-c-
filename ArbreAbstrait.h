#ifndef ARBREABSTRAIT_H
#define ARBREABSTRAIT_H

// Contient toutes les déclarations de classes nécessaires
//  pour représenter l'arbre abstrait

#include <vector>
#include <iostream>
#include <iomanip>
#include <typeinfo>
using namespace std;

#include "Symbole.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
class Noeud {
// Classe abstraite dont dériveront toutes les classes servant à représenter l'arbre abstrait
// Remarque : la classe ne contient aucun constructeur
  public:
    virtual int  executer() =0 ; // Méthode pure (non implémentée) qui rend la classe abstraite
    virtual void ajoute(Noeud* instruction) { throw OperationInterditeException(); }
    virtual ~Noeud() {} // Présence d'un destructeur virtuel conseillée dans les classes abstraites
    virtual void traduitEnCPP(std::ostream & sortie, unsigned int indentation) const {
        sortie << setw(indentation*4) << "" << "traduit en CPP pas défini pour la classe "<<typeid(*this).name() << endl;
    }
};

////////////////////////////////////////////////////////////////////////////////
class NoeudSeqInst : public Noeud {
// Classe pour représenter un noeud "sequence d'instruction"
//  qui a autant de fils que d'instructions dans la séquence
  public:
     NoeudSeqInst();   // Construit une séquence d'instruction vide
    ~NoeudSeqInst() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();    // Exécute chaque instruction de la séquence
    void ajoute(Noeud* instruction);  // Ajoute une instruction à la séquence
    void traduitEnCPP(std::ostream & sortie, unsigned int indentation) const;
    
  private:
    vector<Noeud *> m_instructions; // pour stocker les instructions de la séquence
};

////////////////////////////////////////////////////////////////////////////////
class NoeudAffectation : public Noeud {
// Classe pour représenter un noeud "affectation"
//  composé de 2 fils : la variable et l'expression qu'on lui affecte
  public:
     NoeudAffectation(Noeud* variable, Noeud* expression); // construit une affectation
    ~NoeudAffectation() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();        // Exécute (évalue) l'expression et affecte sa valeur à la variable
    void traduitEnCPP(std::ostream & sortie, unsigned int indentation) const;

  private:
    Noeud* m_variable;
    Noeud* m_expression;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudOperateurBinaire : public Noeud {
// Classe pour représenter un noeud "opération binaire" composé d'un opérateur
//  et de 2 fils : l'opérande gauche et l'opérande droit
  public:
    NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit);
    // Construit une opération binaire : operandeGauche operateur OperandeDroit
   ~NoeudOperateurBinaire() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();            // Exécute (évalue) l'opération binaire)
    void traduitEnCPP(std::ostream & sortie, unsigned int indentation) const;

  private:
    Symbole m_operateur;
    Noeud*  m_operandeGauche;
    Noeud*  m_operandeDroit;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstSi : public Noeud {
// Classe pour représenter un noeud "instruction si"
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
  public:
    NoeudInstSi(Noeud* condition, Noeud* sequence);
     // Construit une "instruction si" avec sa condition et sa séquence d'instruction
   ~NoeudInstSi() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstTantQue : public Noeud {
// Classe pour représenter un noeud "instruction tantque"
//  et ses 2 fils : la condition du tantque et la séquence d'instruction associée
  public:
    NoeudInstTantQue(Noeud* condition, Noeud* sequence);
     // Construit une "instruction tantque" avec sa condition et sa séquence d'instruction
   ~NoeudInstTantQue() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction tantque : tantque condition vraie on exécute la séquence
    void traduitEnCPP(std::ostream & sortie, unsigned int indentation) const;

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstRepeter: public Noeud {
// Classe pour représenter un noeud "instruction repeter"
//  et ses 2 fils : la séquence d'instruction a repeter et la condition de sortie
  public:
    NoeudInstRepeter(Noeud* condition, Noeud* sequence);
     // Construit une "instruction tantque" avec sa condition et sa séquence d'instruction
    ~NoeudInstRepeter() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction tantque : tantque condition vraie on exécute la séquence
    void traduitEnCPP(std::ostream & sortie, unsigned int indentation) const;

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};
////////////////////////////////////////////////////////////////////////////////
class NoeudInstPour: public Noeud {
// Classe pour représenter un noeud "instruction pour"
//  et ses 4 fils : l'affectation, la condition de sortie, l'action a faire en fin de boucle, et la sequence d'actions a repeter
  public:
    NoeudInstPour(Noeud* condition, Noeud* sequence,Noeud* affecter, Noeud* action);
     // Construit une "instruction tantque" avec sa condition et sa séquence d'instruction, affecter et action sont a la fin afin d'avoir des valeurs par défaut
    ~NoeudInstPour() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction tantque : tantque condition vraie on exécute la séquence
    void traduitEnCPP(std::ostream & sortie, unsigned int indentation) const;

  private:
    Noeud*  m_affecter;
    Noeud*  m_condition;
    Noeud*  m_action;
    Noeud*  m_sequence;
};


////////////////////////////////////////////////////////////////////////////////
class NoeudInstEcrire: public Noeud {
// Classe pour représenter un noeud "instruction ecrire"
//    <instEcrire>  ::= ecrire( <expression> | <chaine> {, <expression> | <chaine> })

  public:
    NoeudInstEcrire();
     // Construit une "instruction ecrire" 
    ~NoeudInstEcrire(){}// A cause du destructeur virtuel de la classe Noeud
    void ajoute(Noeud* parametre);
    int executer();  // Exécute l'instruction ecrire
    void traduitEnCPP(std::ostream & sortie, unsigned int indentation) const;

  private:
      vector<Noeud*> m_vecteurEcrire;
};
////////////////////////////////////////////////////////////////////////////////
class NoeudInstSiRiche: public Noeud {
// Classe pour représenter un noeud "instruction siRiche"
//    <instEcrire>  ::= ecrire( <expression> | <chaine> {, <expression> | <chaine> })

  public:
    NoeudInstSiRiche();
     // Construit une "instruction siRiche" avec sa condition et sa séquence d'instruction
    ~NoeudInstSiRiche() {} // A cause du destructeur virtuel de la classe Noeud
    void ajouter(Noeud* instruction);
    int executer();  // Exécute l'instruction siRiche
    void traduitEnCPP(std::ostream & sortie, unsigned int indentation) const;    

  private:
      vector<Noeud*> m_vecteurConditonInstruction;

};
////////////////////////////////////////////////////////////////////////////////
class NoeudInstLire:public Noeud{
    // <instLire>::= lire ( <variable> {,<variable>} )
public:
    NoeudInstLire();
    int executer() override;
    void ajoute(Noeud* variable) override;
    virtual ~NoeudInstLire() {}
    void traduitEnCPP(std::ostream & sortie, unsigned int indentation) const;
    
private:
    vector<Noeud*> m_varLire;
};



#endif /* ARBREABSTRAIT_H */
