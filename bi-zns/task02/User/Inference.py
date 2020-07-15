from typing import List

from ExpertSystem.Business.UserFramework import IInference, ActionBaseCaller
from ExpertSystem.Structure.Enums import LogicalOperator, Operator
from ExpertSystem.Structure.RuleBase import Rule, Fact, ExpressionNode, Expression


class Inference(IInference):
    """
    | User definition of the inference. You can define here you inference method (forward or backward).
      You can have here as many functions as you want, but you must implement interfere with same signature

    |
    | `def interfere(self, knowledge_base: List[Fact], rules: List[Rule], action_base: ActionBase):`
    |

    | Method `interfere` will be called each turn or manually with `Inference` button.
    | Class have no class parameters, you can use only inference parameters

    """
    knowledge_base: List[Fact]
    action_base: ActionBaseCaller


    def infere(self, knowledge_base: List[Fact], rules: List[Rule], action_base: ActionBaseCaller) -> None:
        """
        User defined inference

        :param knowledge_base: - list of Fact classes defined in  KnowledgeBase.create_knowledge_base()
        :param rules:  - list of rules trees defined in rules file.
        :param action_base: - instance of ActionBaseCaller for executing conclusions

        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        !!    TODO: Write implementation of your inference mechanism definition HERE    !!
        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        """
        self.knowledge_base = knowledge_base
        self.action_base = action_base
        while True:
            valid_rules = []
            for rule in rules:
                condition = self.rule_evaluation(rule.condition)

                #if rule.uncertainty:
                #    print(f'Rule {rule} has uncertainty {rule.uncertainty}')

                if condition:
                    condition *= rule.uncertainty
                    valid_rules.append((condition, rule))
            if not valid_rules:
                return
            best_rule = max(valid_rules, key=lambda x: x[0])
            if best_rule:
                self.conclusion_evaluation(best_rule[1].conclusion)



    def rule_evaluation(self, root_node: ExpressionNode) -> int:
        """
        Example of rule tree evaluation. This implementation did not check comparision operators and uncertainty.
        For usage in inference extend this function

        :param root_node: root node of the rule tree
        :return: True if rules is satisfiable, False in case of not satisfiable or missing Facts
        """

        if root_node.operator == LogicalOperator.AND:
            # print("AND returning", min(self.rule_evaluation(root_node.left),  self.rule_evaluation(root_node.right)))
            return min(self.rule_evaluation(root_node.left),  self.rule_evaluation(root_node.right))

        elif root_node.operator == LogicalOperator.OR:
            # print("OR returning", max(self.rule_evaluation(root_node.left), self.rule_evaluation(root_node.right)))
            return max(self.rule_evaluation(root_node.left), self.rule_evaluation(root_node.right))

        elif isinstance(root_node.value, Expression):
            if root_node.value.uncertainty:
                print(f'Expression {root_node.value.name} has {root_node.value.uncertainty} uncertainty')
            try:
                fact_prob = self.knowledge_base[self.knowledge_base.index(root_node.value.name)].probability

                res = self.knowledge_base[self.knowledge_base.index(root_node.value.name)](*root_node.value.args)
                if root_node.value.comparator is None:
                    if res:
                        # print("Inference fact_prob", fact_prob)
                        return fact_prob
                    return 0
                elif root_node.value.comparator is Operator.LESS_THEN:
                    if res < int(root_node.value.value):
                        # print("Inference fact_prob", fact_prob)
                        return fact_prob
                    return 0
                elif root_node.value.comparator is Operator.MORE_THEN:
                    if res > int(root_node.value.value):
                        # print("Inference fact_prob", fact_prob)
                        return fact_prob
                    return 0
                elif root_node.value.comparator is Operator.LESS_EQUAL:
                    if res <= int(root_node.value.value):
                        # print("Inference fact_prob", fact_prob)
                        return fact_prob
                    return 0
                elif root_node.value.comparator is Operator.MORE_EQUAL:
                    if res >= int(root_node.value.value):
                        # print("Inference fact_prob", fact_prob)
                        return fact_prob
                    return 0
                else:
                    print("Undefined comparison")
            except ValueError:
                return 0

        else:
            return 0  # should not happen


    def conclusion_evaluation(self, root_node: ExpressionNode):
        if self.action_base.has_method(root_node.value):
            self.action_base.call(root_node.value)
