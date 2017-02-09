#ifndef SEQUENCE_LIST_PARAM_CONTROLLER_H
#define SEQUENCE_LIST_PARAM_CONTROLLER_H

#include "../../shared/list_parameter_controller.h"
#include "../sequence.h"
#include "../sequence_store.h"

namespace Sequence {

class ListParameterController : public Shared::ListParameterController {
public:
  ListParameterController(Responder * parentResponder, SequenceStore * sequenceStore);
  ~ListParameterController();
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void setSequence(Sequence * sequence);
  int numberOfRows() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  constexpr static int k_totalNumberOfCell = 4;
  ChevronExpressionMenuListCell m_typeCell;
  Poincare::ExpressionLayout * m_typeLayout;
};

}

#endif
