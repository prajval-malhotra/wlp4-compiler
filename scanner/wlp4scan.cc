#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <utility>
#include <set>
#include <array>
#include <climits>
#include "wlp4scan.h"
using namespace std;

Token::Token(Token::Kind kind, std::string lexeme):
  kind(kind), lexeme(std::move(lexeme)) {}

  Token:: Kind Token::getKind() const { return kind; }

void Token::setKind(Token::Kind new_kind) {
  kind = new_kind;
}

const std::string &Token::getLexeme() const { return lexeme; }

std::ostream &operator<<(std::ostream &out, const Token &tok) {
  switch (tok.getKind()) {
    case Token::ID:         out << "ID";         break;
    case Token::NUM:      out << "NUM";      break;
    case Token::LPAREN:     out << "LPAREN";     break;
    case Token::RPAREN:     out << "RPAREN";     break;
    case Token::LBRACE:        out << "LBRACE";        break;
    case Token::RBRACE: out << "RBRACE"; break;
    case Token::RETURN:    out << "RETURN";    break;
    case Token::IF:         out << "IF";         break;
    case Token::ELSE:      out << "ELSE";      break;
    case Token::WHILE:     out << "WHILE";     break;
    case Token::PRINTLN:     out << "PRINTLN";     break;
    case Token::WAIN:        out << "WAIN";        break;
    case Token::BECOMES: out << "BECOMES"; break;
    case Token::INT:    out << "INT";    break;
    case Token::EQ:         out << "EQ";         break;
    case Token::NE:      out << "NE";      break;
    case Token::LT:     out << "LT";     break;
    case Token::GT:     out << "GT";     break;
    case Token::LE:        out << "LE";        break;
    case Token::GE:        out << "GE";        break;
    case Token::PLUS: out << "PLUS"; break;
    case Token::MINUS:    out << "MINUS";    break;
    case Token::STAR:         out << "STAR";         break;
    case Token::SLASH:      out << "SLASH";      break;
    case Token::PCT:     out << "PCT";     break;
    case Token::COMMA:     out << "COMMA";     break;
    case Token::SEMI:        out << "SEMI";        break;
    case Token::NEW: out << "NEW"; break;
    case Token::DELETE:    out << "DELETE";    break;
    case Token::LBRACK:         out << "LBRACK";         break;
    case Token::RBRACK:      out << "RBRACK";      break;
    case Token::AMP:     out << "AMP";     break;
    case Token::NUL:     out << "NULL";     break;
    case Token::ZERO:     out << "ZERO";     break;
    case Token::WHITESPACE:     break;
    case Token::COMMENT:    break;
  }
  std::cout << " "; 
  out << tok.getLexeme();

  return out;
}

int64_t Token::toNumber() const {
  std::istringstream iss;
  int64_t result;

  if (kind == NUM) {
    iss.str(lexeme);
  } 
  else {
    // This should never happen if the user calls this function correctly
    return 0;
  }

  iss >> result;
  return result;
}

ScanningFailure::ScanningFailure(std::string message):
  message(std::move(message)) {}

const std::string &ScanningFailure::what() const { return message; }

/* Representation of a DFA, used to handle the scanning process.
 */
class wlp4DFA {
  public:
    enum State {
      // States that are also kinds
      ID=0,
      NUM,
      LPAREN,
      RPAREN,
      LBRACE,
      RBRACE,
      RETURN,
      IF,
      ELSE,
      WHILE,
      PRINTLN,
      WAIN,
      BECOMES,
      INT,
      EQ,
      NE,
      LT,
      GT,
      LE,
      GE,
      PLUS,
      MINUS,
      STAR,
      SLASH,
      PCT,
      COMMA,
      SEMI,
      NEW,
      DELETE,
      LBRACK,
      RBRACK,
      AMP,
      NUL,
      WHITESPACE,
      COMMENT,
      ZERO,

      // States that are not also kinds
      FAIL,
      START,
      TEMP,
      // DOT,
      // DOTID,
      STATE_MINUS,
      DOLLARS,

      // Hack to let this be used easily in arrays. This should always be the
      // final element in the enum, and should always point to the previous
      // element.

      LARGEST_STATE = DOLLARS
    };

  private:
    /* A set of all accepting states for the DFA.
     * Currently non-accepting states are not actually present anywhere
     * in memory, but a list can be found in the constructor.
     */
    std::set<State> acceptingStates;

    /*
     * The transition function for the DFA, stored as a map.
     */

    std::array<std::array<State, 128>, LARGEST_STATE + 1> transitionFunction;

    /*
     * Converts a state to a kind to allow construction of Tokens from States.
     * Throws an exception if conversion is not possible.
     */
    Token::Kind stateToKind(State s) const {
      switch(s) {
        case Token::ID: return Token::ID;
        case Token::NUM:     return Token::NUM;
        case Token::LPAREN: return Token::LPAREN;
        case Token::RPAREN: return Token::RPAREN;
        case Token::LBRACE: return Token::LBRACE;
        case Token::RBRACE: return Token::RBRACE;
        case Token::RETURN: return Token::RETURN;
        case Token::IF:    return Token::IF;
        case Token::ELSE:  return Token::ELSE;
        case Token::WHILE: return Token::WHILE;
        case Token::PRINTLN:  return Token::PRINTLN;
        case Token::WAIN:   return Token::WAIN;
        case Token::BECOMES: return Token::BECOMES;
        case Token::INT:    return Token::INT;
        case Token::EQ:     return Token::EQ;
        case Token::NE:     return Token::NE;
        case Token::LT:     return Token::LT;
        case Token::GT:     return Token::GT;
        case Token::LE:     return Token::LE;
        case Token::GE:     return Token::GE;
        case Token::PLUS:    return Token::PLUS;
        case Token::MINUS:   return Token::MINUS;
        case Token::STAR:    return Token::STAR;
        case Token::SLASH:   return Token::SLASH;
        case Token::PCT:    return Token::PCT;
        case Token::COMMA:   return Token::COMMA;
        case Token::SEMI:    return Token::SEMI;
        case Token::NEW: return Token::NEW;
        case Token::DELETE: return Token::DELETE;
        case Token::LBRACK: return Token::LBRACK;
        case Token::RBRACK: return Token::RBRACK;
        case Token::AMP:  return Token::AMP;
        case Token::NUL:    return Token::NUL;
        case Token::WHITESPACE:    return Token::WHITESPACE;
        case Token::COMMENT:    return Token::COMMENT;
        case Token::ZERO:    return Token::ZERO;
        default: {
          throw ScanningFailure("ERROR: Cannot convert state to kind.");}
      }
    }


  public:
    /* Tokenizes an input string according to the Simplified Maximal Munch
     * scanning algorithm.
     */
    std::vector<Token> simplifiedMaximalMunch(const std::string &input) const {
      std::vector<Token> result;

      State state = start();
      std::string munchedInput;

      // We can't use a range-based for loop effectively here
      // since the iterator doesn't always increment.
      for (std::string::const_iterator inputPosn = input.begin();
           inputPosn != input.end();) {

        State oldState = state;
        state = transition(state, *inputPosn);

        if (!failed(state)) {
          munchedInput += *inputPosn;
          oldState = state;

          ++inputPosn;
        }

        if (inputPosn == input.end() || failed(state)) {
          if (accept(oldState)) {
            result.push_back(Token(stateToKind(oldState), munchedInput));

            munchedInput = "";
            state = start();
          } else {
            if (failed(state)) {
              munchedInput += *inputPosn;
            }
            throw ScanningFailure("ERROR: Simplified maximal munch failed on input: "
                                 + munchedInput);
          }
        }
      }

      return result;
    }

    /* Initializes the accepting states for the DFA.
     */
    wlp4DFA() {
      acceptingStates = {
        ID,
        NUM,
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        RETURN,
        IF,
        ELSE,
        WHILE,
        PRINTLN,
        WAIN,
        BECOMES,
        INT,
        EQ,
        NE,
        LT,
        GT,
        LE,
        GE,
        PLUS,
        MINUS,
        STAR,
        SLASH,
        PCT,
        COMMA,
        SEMI,
        NEW,
        DELETE,
        LBRACK,
        RBRACK,
        AMP,
        NUL,
        ZERO,
        WHITESPACE,
        COMMENT,
      };
      // Initialize transitions for the DFA
      for (size_t i = 0; i < transitionFunction.size(); ++i) {
        for (size_t j = 0; j < transitionFunction[0].size(); ++j) {
          transitionFunction[i][j] = FAIL;
        }
      }

      registerTransition(START, isalpha, ID);
      registerTransition(START, "0", ZERO);
      // registerTransition(START, "0", NUM);
      registerTransition(START, "123456789", NUM);
      registerTransition(NUM, "0123456789", NUM);
      registerTransition(START, "+", PLUS);
      registerTransition(START, "-", MINUS);
      registerTransition(START, "*", STAR);
      registerTransition(START, "/", SLASH);
      registerTransition(START, "%", PCT);
      registerTransition(START, ",", COMMA);
      registerTransition(START, ";", SEMI);
      registerTransition(START, "&", AMP);
      registerTransition(SLASH, "/", COMMENT);
      registerTransition(START, "=", BECOMES);
      registerTransition(BECOMES, "=", EQ);
      registerTransition(START, "!", TEMP);
      registerTransition(TEMP, "=", NE);
      registerTransition(START, "<", LT);
      registerTransition(START, ">", GT);
      registerTransition(LT, "=", LE);
      registerTransition(GT, "=", GE);
      registerTransition(START, isspace, WHITESPACE);
      registerTransition(START, "$", DOLLARS);
      registerTransition(START, "(", LPAREN);
      registerTransition(START, ")", RPAREN);
      registerTransition(START, "{", LBRACE);
      registerTransition(START, "}", RBRACE);
      registerTransition(START, "[", LBRACK);
      registerTransition(START, "]", RBRACK);
      registerTransition(ID, isalnum, ID);
      registerTransition(ZERO, isdigit, NUM);
      registerTransition(NUM, isdigit, NUM);
      registerTransition(COMMENT, [](int c) -> int { return c != '\n'; },
          COMMENT);
      registerTransition(WHITESPACE, isspace, WHITESPACE);
    }

    // Register a transition on all chars in chars
    void registerTransition(State oldState, const std::string &chars,
        State newState) {
      for (char c : chars) {
        transitionFunction[oldState][c] = newState;
      }
    }

    // Register a transition on all chars matching test
    // For some reason the cctype functions all use ints, hence the function
    // argument type.
    void registerTransition(State oldState, int (*test)(int), State newState) {

      for (int c = 0; c < 128; ++c) {
        if (test(c)) {
          transitionFunction[oldState][c] = newState;
        }
      }
    }

    /* Returns the state corresponding to following a transition
     * from the given starting state on the given character,
     * or a special fail state if the transition does not exist.
     */
    State transition(State state, char nextChar) const {
      return transitionFunction[state][nextChar];
    }

    /* Checks whether the state returned by transition
     * corresponds to failure to transition.
     */
    bool failed(State state) const { return state == FAIL; }

    /* Checks whether the state returned by transition
     * is an accepting state.
     */
    bool accept(State state) const {
      return acceptingStates.count(state) > 0;
    }

    /* Returns the starting state of the DFA
     */
    State start() const { return START; }
};

std::vector<Token> scan(const std::string &input) {
  static wlp4DFA theDFA;

  std::vector<Token> tokens = theDFA.simplifiedMaximalMunch(input);

  // We need to:
  // * Throw exceptions for WORD tokens whose lexemes aren't ".word".
  // * Remove WHITESPACE and COMMENT tokens entirely.

  std::vector<Token> newTokens;

  for (auto &token : tokens) {
    if (token.getKind() == Token::NUM) {
    }
    if (token.getKind() == Token::ID) {
      if (token.getLexeme() == "wain") {
        token.setKind(Token::WAIN);
      }
      if (token.getLexeme() == "int") {
        token.setKind(Token::INT);
      }
      if (token.getLexeme() == "return") {
        token.setKind(Token::RETURN);
      }
      if (token.getLexeme() == "if") {
        token.setKind(Token::IF);
      }
      if (token.getLexeme() == "else") {
        token.setKind(Token::ELSE);
      }
      if (token.getLexeme() == "println") {
        token.setKind(Token::PRINTLN);
      }
      if (token.getLexeme() == "while") {
        token.setKind(Token::WHILE);
      }
      if (token.getLexeme() == "new") {
        token.setKind(Token::NEW);
      }
      if (token.getLexeme() == "delete") {
        token.setKind(Token::DELETE);
      }
      if (token.getLexeme() == "NULL") {
        token.setKind(Token::NUL);
      }
    } 
    if (token.getKind() != Token::WHITESPACE
        && token.getKind() != Token::Kind::COMMENT) {
      newTokens.push_back(token);
    }
  }

  return newTokens;
}
