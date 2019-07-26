#ifndef POINCARE_EXPRESSION_NODE_H
#define POINCARE_EXPRESSION_NODE_H

#include <poincare/tree_node.h>
#include <poincare/evaluation.h>
#include <poincare/layout.h>
#include <poincare/context.h>
#include <stdint.h>

namespace Poincare {

/* Methods preceded by '*!*' interfere with the expression pool, which can make
 * 'this' outdated. They should only be called in a wrapper on Expression. */

class SymbolAbstract;
class Symbol;
class ComplexCartesian;
class ComplexPolar;

class ExpressionNode : public TreeNode {
  friend class AdditionNode;
  friend class DivisionNode;
  friend class NAryExpressionNode;
  friend class PowerNode;
  friend class SymbolNode;
public:
   enum class Type : uint8_t {
    Uninitialized = 0,
    Undefined = 1,
    Unreal,
    Rational,
    Decimal,
    Float,
    Infinity,
    MultiplicationExplicite,
    MultiplicationImplicite,
    Power,
    Addition,
    Factorial,
    Division,
    Constant,
    Symbol,
    Store,
    Equal,
    Sine,
    Cosine,
    Tangent,
    AbsoluteValue,
    ArcCosine,
    ArcSine,
    ArcTangent,
    BinomialCoefficient,
    Ceiling,
    ComplexArgument,
    ComplexPolar,
    Conjugate,
    Derivative,
    Determinant,
    DivisionQuotient,
    DivisionRemainder,
    Factor,
    Floor,
    FracPart,
    Function,
    GreatCommonDivisor,
    HyperbolicArcCosine,
    HyperbolicArcSine,
    HyperbolicArcTangent,
    HyperbolicCosine,
    HyperbolicSine,
    HyperbolicTangent,
    ImaginaryPart,
    Integral,
    LeastCommonMultiple,
    Logarithm,
    MatrixTrace,
    NaperianLogarithm,
    NthRoot,
    Opposite,
    Parenthesis,
    PermuteCoefficient,
    Product,
    Random,
    Randint,
    RealPart,
    Round,
    SignFunction,
    SquareRoot,
    Subtraction,
    Sum,

    ComplexCartesian,

    ConfidenceInterval,
    MatrixDimension,
    MatrixIdentity,
    MatrixInverse,
    MatrixTranspose,
    PredictionInterval,
    Matrix,
    EmptyExpression
   };

  /* Poor man's RTTI */
  virtual Type type() const = 0;

  /* Properties */
  enum class ReductionTarget {
    System = 0,
    User
  };
  enum class Sign {
    Negative = -1,
    Unknown = 0,
    Positive = 1
  };

  class ReductionContext {
  public:
    ReductionContext(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation = true) :
      m_context(context),
      m_complexFormat(complexFormat),
      m_angleUnit(angleUnit),
      m_target(target),
      m_symbolicComputation(symbolicComputation)
    {}
    Context * context() { return m_context; }
    Preferences::ComplexFormat complexFormat() const { return m_complexFormat; }
    Preferences::AngleUnit angleUnit() const { return m_angleUnit; }
    ReductionTarget target() const { return m_target; }
    bool symbolicComputation() const { return m_symbolicComputation; }
  private:
    Context * m_context;
    Preferences::ComplexFormat m_complexFormat;
    Preferences::AngleUnit m_angleUnit;
    ReductionTarget m_target;
    bool m_symbolicComputation;
  };

  virtual Sign sign(Context * context) const { return Sign::Unknown; }
  virtual bool isNumber() const { return false; }
  virtual bool isRandom() const { return false; }
  virtual bool isParameteredExpression() const { return false; }
  /* childNeedsUserParentheses check if parentheses are required by mathematical rules:
   * +(2,-1) --> 2+(-1)
   * *(+(2,1),3) --> (2+1)*3
   */
  virtual bool childNeedsUserParentheses(const Expression & child) const { return false; }
  /*!*/ virtual Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression);
  /*!*/ virtual Expression replaceUnknown(const Symbol & symbol, const Symbol & unknownSymbol);
  /*!*/ virtual Expression setSign(Sign s, ReductionContext reductionContext);
  virtual int polynomialDegree(Context * context, const char * symbolName) const;
  /*!*/ virtual int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const;
  /*!*/ virtual Expression shallowReplaceReplaceableSymbols(Context * context);
  typedef bool (*isVariableTest)(const char * c);
  virtual int getVariables(Context * context, isVariableTest isVariable, char * variables, int maxSizeVariable) const;
  virtual float characteristicXRange(Context * context, Preferences::AngleUnit angleUnit) const;
  bool isOfType(Type * types, int length) const;

  /* Complex */
  virtual bool isReal(Context * context) const { return false; }

  /* Simplification */
  /* SimplificationOrder returns:
   *   1 if e1 > e2
   *   -1 if e1 < e2
   *   0 if e1 == e
   * The order groups like terms together to avoid quadratic complexity when
   * factorizing addition or multiplication. For example, it groups terms with
   * same bases together (ie Pi, Pi^3)  and with same non-rational factors
   * together (ie Pi, 2*Pi).
   * Because SimplificationOrder is a recursive call, we sometimes enable its
   * interruption to avoid freezing in the simplification process. */
  static int SimplificationOrder(const ExpressionNode * e1, const ExpressionNode * e2, bool ascending, bool canBeInterrupted = false);
  /* In the simplification order, most expressions are compared by only
   * comparing their types. However hierarchical expressions of same type would
   * compare their operands and thus need to reimplement
   * simplificationOrderSameType. Besides, operations that can be simplified
   * (ie +, *, ^, !) have specific rules to group like terms together and thus
   * reimplement simplificationOrderGreaterType. */
  virtual int simplificationOrderGreaterType(const ExpressionNode * e, bool ascending, bool canBeInterrupted) const { return ascending ? -1 : 1; }
  virtual int simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted) const;

  /* Layout Helper */
  virtual Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const = 0;

  /* Evaluation Helper */
  typedef float SinglePrecision;
  typedef double DoublePrecision;
  constexpr static int k_maxNumberOfSteps = 10000;
  virtual Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const = 0;
  virtual Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const = 0;

  /* Simplification */
  /*!*/ virtual void deepReduceChildren(ReductionContext reductionContext);
  /*!*/ virtual Expression shallowReduce(ReductionContext reductionContext);
  /*!*/ virtual Expression shallowBeautify(ReductionContext reductionContext);
  /* Return a clone of the denominator part of the expression */
  /*!*/ virtual Expression denominator(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

  /* Hierarchy */
  ExpressionNode * childAtIndex(int i) const override { return static_cast<ExpressionNode *>(TreeNode::childAtIndex(i)); }
  virtual void setChildrenInPlace(Expression other);

protected:
  /* Hierarchy */
  ExpressionNode * parent() const override { return static_cast<ExpressionNode *>(TreeNode::parent()); }
  Direct<ExpressionNode> children() const { return Direct<ExpressionNode>(this); }
};

}

#endif
