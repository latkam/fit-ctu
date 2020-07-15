from typing import List, Set

from OrodaelTurrim.Business.Interface.Player import IPlayer
from OrodaelTurrim.Business.Proxy import MapProxy, GameObjectProxy, GameUncertaintyProxy
from ExpertSystem.Business.UserFramework import IKnowledgeBase
from ExpertSystem.Structure.RuleBase import Fact
from OrodaelTurrim.Structure.Position import OffsetPosition

class KnowledgeBase(IKnowledgeBase):
    """
    Class for defining known facts based on Proxy information. You can transform here any information from
    proxy to better format of Facts. Important is method `create_knowledge_base()`. Return value of this method
    will be passed to `Interference.interfere`. It is recommended to use Fact class but you can use another type.

    |
    |
    | Class provides attributes:

    - **map_proxy [MapProxy]** - Proxy for access to map information
    - **game_object_proxy [GameObjectProxy]** - Proxy for access to all game object information
    - **uncertainty_proxy [UncertaintyProxy]** - Proxy for access to all uncertainty information in game
    - **player [IPlayer]** - instance of user player for identification in proxy methods

    """
    map_proxy: MapProxy
    game_object_proxy: GameObjectProxy
    game_uncertainty_proxy: GameUncertaintyProxy
    player: IPlayer




    def __init__(self, map_proxy: MapProxy, game_object_proxy: GameObjectProxy,
                 game_uncertainty_proxy: GameUncertaintyProxy, player: IPlayer):
        """
        You can add some code to __init__ function, but don't change the signature. You cannot initialize
        KnowledgeBase class manually so, it is make no sense to change signature.
        """
        super().__init__(map_proxy, game_object_proxy, game_uncertainty_proxy, player)


    def create_knowledge_base(self) -> List[Fact]:
        """
        Method for create user knowledge base. You can also have other class methods, but entry point must be this
        function. Don't change the signature of the method, you can change return value, but it is not recommended.

        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        !!  TODO: Write implementation of your knowledge base definition HERE   !!
        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        """

        facts = []

        visible_tiles = self.map_proxy.get_player_visible_tiles()

        facts.append(Fact('money', self.get_resources))

        if not self.map_proxy.player_have_base(self.player):
            facts.append(Fact('player_dont_have_base'))

        if self.map_proxy.player_have_base(self.player):
            facts.append((Fact('player_have_base')))

        for pos in visible_tiles:
            facts.append(Fact('position_is_visible', self.position_is_visible))
            if not self.map_proxy.is_position_occupied(pos):
                facts.append(Fact('position_is_not_occupied', self.position_is_not_occupied))
            if self.map_proxy.is_position_occupied(pos):
                facts.append(Fact('position_is_occupied', self.position_is_occupied))
        return facts

    def position_is_occupied(self, x, y):
        return self.map_proxy.is_position_occupied(OffsetPosition(int(x), int(y)))

    def position_is_not_occupied(self, x, y):
        return not self.map_proxy.is_position_occupied(OffsetPosition(int(x), int(y)))

    def get_resources(self):
        return self.game_object_proxy.get_resources(self.player)

    def position_is_visible(self, x, y):
        return OffsetPosition(int(x), int(y)) in self.map_proxy.get_player_visible_tiles()
