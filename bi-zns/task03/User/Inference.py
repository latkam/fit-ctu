from typing import List
from math import floor

from ExpertSystem.Business.UserFramework import IInference, ActionBaseCaller
from ExpertSystem.Structure.Enums import LogicalOperator, Operator
from ExpertSystem.Structure.RuleBase import Rule, Fact, ExpressionNode, Expression
from OrodaelTurrim.Structure.Position import OffsetPosition


fuzzy_place_weak_guards    = {"many": 0, "average": 0, "few": 0}
fuzzy_place_strong_guards  = {"many": 0, "average": 0, "few": 0}
defuzzy_place_weak_guards = 0
defuzzy_place_strong_guards = 0


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
        #print("="*100)
        self.knowledge_base = knowledge_base
        self.action_base = action_base

        for rule in rules:
            #print("rule:", rule)
            condition = self.rule_evaluation(rule.condition)
            #print("g_condition first", condition)

            if condition:
                self.conclusion_evaluation(rule.conclusion, condition)



    def rule_evaluation(self, root_node: ExpressionNode) -> float:
        """
        Example of rule tree evaluation. This implementation did not check comparision operators and uncertainty.
        For usage in inference extend this function

        :param root_node: root node of the rule tree
        :return: True if rules is satisfiable, False in case of not satisfiable or missing Facts
        """

        if root_node.operator == LogicalOperator.AND:
            if (isinstance(self.rule_evaluation(root_node.left), int) or isinstance(self.rule_evaluation(root_node.left), float)) and (isinstance(self.rule_evaluation(root_node.right), int) or isinstance(self.rule_evaluation(root_node.right), float)):
                return min(float(self.rule_evaluation(root_node.left)), float(self.rule_evaluation(root_node.right)))
            else:
                return self.rule_evaluation(root_node.left) and self.rule_evaluation(root_node.right)

        elif root_node.operator == LogicalOperator.OR:
            if (isinstance(self.rule_evaluation(root_node.left), int) or isinstance(self.rule_evaluation(root_node.left), float)) and (isinstance(self.rule_evaluation(root_node.right), int) or isinstance(self.rule_evaluation(root_node.right), float)):
                return max(float(self.rule_evaluation(root_node.left)), float(self.rule_evaluation(root_node.right)))
            else:
                return self.rule_evaluation(root_node.left) or self.rule_evaluation(root_node.right)

        elif isinstance(root_node.value, Expression):
            if root_node.value.uncertainty:
                print(f'Expression {root_node.value.name} has {root_node.value.uncertainty} uncertainty')
            try:
                res = self.knowledge_base[self.knowledge_base.index(root_node.value.name)](*root_node.value.args)
                if root_node.value.comparator is None:
                    return res
                elif root_node.value.comparator is Operator.EQUAL:
                    return res is root_node.value.value
                elif root_node.value.comparator is Operator.NOT_EQUAL:
                    return res is not root_node.value.value
                elif root_node.value.comparator is Operator.LESS_THEN:
                    return res < int(root_node.value.value)
                elif root_node.value.comparator is Operator.MORE_THEN:
                    return res > int(root_node.value.value)
                elif root_node.value.comparator is Operator.LESS_EQUAL:
                    return res <= int(root_node.value.value)
                elif root_node.value.comparator is Operator.MORE_EQUAL:
                    return res >= int(root_node.value.value)
                else:
                    print("Undefined comparison")
            except ValueError:
                return False

        else:
            return bool(root_node.value)  # should not happen


    def conclusion_evaluation(self, root_node: ExpressionNode, condition):
        global defuzzy_place_weak_guards
        global defuzzy_place_strong_guards
        if root_node.value.comparator is Operator.ASSIGN:
            if root_node.value.name == "fuzzy_place_weak_guards":
                fuzzy_place_weak_guards[root_node.value.value] = condition
            elif root_node.value.name == "fuzzy_place_strong_guards":
                #print("assign to fuzzy_place_strong_guards")
                fuzzy_place_strong_guards[root_node.value.value] = condition
            #print("After assigning to fuzzy_place")
            #print("fuzzy_place_weak_guards: ", fuzzy_place_weak_guards)
            #print("fuzzy_place_strong_guards: ", fuzzy_place_strong_guards)

            defuzzy_place_weak_guards = self.defuzzificate_place_guards(fuzzy_place_weak_guards)
            defuzzy_place_strong_guards = self.defuzzificate_place_guards(fuzzy_place_strong_guards)
        if self.action_base.has_method(root_node.value):
            #print("root node value args", root_node.value.args)
            #print("calling type", type(root_node.value))
            if 'build_magician' in str(root_node.value):
                root_node.value.args.append(defuzzy_place_strong_guards)
            elif 'build_knight' in str(root_node.value):
                root_node.value.args.append(defuzzy_place_weak_guards)
            self.action_base.call(root_node.value)

    def defuzzificate_place_guards(self, place_guards):
        nominator   = 0
        denominator = 0
        for i in range(0, 11):
            nominator   += i*place_guards["few"]
            denominator += place_guards["few"]

        for i in range(8, 27):
            nominator   += i * place_guards["average"]
            denominator += place_guards["average"]

        for i in range(28, 31):
            nominator   += i * place_guards["many"]
            denominator += place_guards["many"]

        if denominator == 0:
            result = 0
        else:
            result = floor(nominator / denominator)
        return result
