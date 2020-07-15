from typing import List
from OrodaelTurrim.Business.Interface.Player import PlayerTag
from OrodaelTurrim.Business.Proxy import MapProxy, GameObjectProxy, GameUncertaintyProxy
from ExpertSystem.Business.UserFramework import IKnowledgeBase
from ExpertSystem.Structure.RuleBase import Fact
from OrodaelTurrim.Structure.Enums import TerrainType, AttributeType, EffectType, GameRole, GameObjectType
from OrodaelTurrim.Structure.Position import OffsetPosition, CubicPosition, AxialPosition


class KnowledgeBase(IKnowledgeBase):
    """
    Class for defining known facts based on Proxy information. You can transform here any information from
    proxy to better format of Facts. Important is method `create_knowledge_base()`. Return value of this method
    will be passed to `Inference.interfere`. It is recommended to use Fact class but you can use another type.

    |
    |
    | Class provides attributes:

    - **map_proxy [MapProxy]** - Proxy for access to map information
    - **game_object_proxy [GameObjectProxy]** - Proxy for access to all game object information
    - **uncertainty_proxy [UncertaintyProxy]** - Proxy for access to all uncertainty information in game
    - **player [PlayerTag]** - class that serve as instance of user player for identification in proxy methods

    """
    map_proxy: MapProxy
    game_object_proxy: GameObjectProxy
    game_uncertainty_proxy: GameUncertaintyProxy
    player: PlayerTag


    def __init__(self, map_proxy: MapProxy, game_object_proxy: GameObjectProxy,
                 game_uncertainty_proxy: GameUncertaintyProxy, player: PlayerTag):
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

        # Add bool fact
        if not self.map_proxy.player_have_base(self.player):
            facts.append(Fact('player_dont_have_base', eval_function=self.player_dont_have_base, data=self.player_dont_have_base))



        # Add fact with data holder
        # We can use there eval function same as data function
        # because if first_free_tile return None, bool value of None is False, otherwise bool value is True
        # You can use different functions for eval and data
        facts.append(Fact('player_has_base', eval_function=self.player_has_base, data=self.player_has_base))
        facts.append(Fact('free_tile', eval_function=self.first_free_tile, data=self.first_free_tile))
        facts.append(Fact('visible_free_tile', eval_function=self.visible_free_tile, data=self.visible_free_tile))
        #facts.append(Fact('base_neighbours', eval_function=self.base_neighbours, data=self.base_neighbours))
        facts.append(Fact('positions', eval_function=self.positions, data=self.positions))
        facts.append(Fact('position_first_round', eval_function=self.position_first_round, data=self.position_first_round))
        facts.append(Fact('position_second_round', eval_function=self.position_second_round, data=self.position_second_round))
        facts.append(Fact('position_third_round', eval_function=self.position_third_round, data=self.position_third_round))
        facts.append(Fact('fuzzy_money', eval_function=self.fuzzy_money, data=self.fuzzy_money))
        facts.append(Fact('fuzzy_nmb_guards', eval_function=self.fuzzy_nmb_guards, data=self.fuzzy_nmb_guards))


        # Add numerical fact
        facts.append(Fact("money", lambda: self.game_object_proxy.get_resources(self.player)))
        facts.append(Fact('current_round', lambda: self.game_object_proxy.get_current_round()))

        # print("money:", self.game_object_proxy.get_resources(self.player))

        return facts

    fuzzy_place_weak_guards = {"many": 0, "average": 0, "few": 0}
    fuzzy_place_average_guards = {"many": 0, "average": 0, "few": 0}
    fuzzy_place_strong_guards = {"many": 0, "average": 0, "few": 0}
    defuzzy_place_weak_guards = 0
    defuzzy_place_strong_guards = 0

    def player_has_base(self):
        return self.map_proxy.player_have_base(self.player)

    def player_dont_have_base(self): # Czenglish, I know
        return not self.map_proxy.player_have_base(self.player)

    def membership_function_low_value(self, c, d, x):  # right part of the trapezoid
        if x <= c:
            return 1
        elif x >= d:
            return 0
        else:
            return ((x - d) / (c - d))

    def membership_function_moderate_value(self, a, b, c, d, x):  # entire trapezoid
        return max(min(((x - a) / (b - a)), 1, ((d - x) / (d - c))), 0)

    def membership_function_high_value(self, a, b, x):  # left part of the trapezoid
        if x <= a:
            return 0
        elif x >= b:
            return 1
        else:
            return ((x - a) / (b - a))

    def fuzzy_money(self, fuzzy_set):
        resources = self.game_object_proxy.get_resources(self.player)
        res = 0
        if fuzzy_set == "poor":
            res = self.membership_function_low_value(0, 70, resources)
        elif fuzzy_set == "middle_class":
            res = self.membership_function_moderate_value(35, 80, 140, 180, resources)
        elif fuzzy_set == "rich":
            res = self.membership_function_high_value(150, 200, resources)
        #print("fuzzy money returning", res)
        return res

    def fuzzy_base_hp(self, fuzzy_set):
        base_HP = self.game_object_proxy.get_current_hit_points(self.map_proxy.get_bases_positions().pop())
        res = 0
        if fuzzy_set == "old":
            res = self.membership_function_low_value(0, 120, base_HP)
        elif fuzzy_set == "middle_age":
            res = self.membership_function_moderate_value(80, 140, 240, 300, base_HP)
        elif fuzzy_set == "young":
            res = self.membership_function_moderate_value(260, 320, 380, 440, base_HP)
        elif fuzzy_set == "new_born":
            res = self.membership_function_high_value(420, 450, base_HP)
        #print("fuzzy_base_hp returning", res)
        return res

    def fuzzy_nmb_guards(self, fuzzy_set):
        guards = self.nmb_guards()
        res = 0
        if fuzzy_set == "few":
            res = self.membership_function_low_value(0, 10, guards)
        elif fuzzy_set == "average":
            res = self.membership_function_moderate_value(8, 12, 22, 26, guards)
        elif fuzzy_set == "many":
            res = self.membership_function_high_value(22, 28, guards)
        #print("fuzzy_nmb_guards returning", res)
        return res

    def nmb_guards(self):
        cnt = 0
        possible_tiles = self.map_proxy.get_player_visible_tiles() - self.map_proxy.get_border_tiles()
        for possible_tile in possible_tiles:
            if self.map_proxy.is_position_occupied(possible_tile) and self.game_object_proxy.get_object_type(possible_tile) is GameObjectType.KNIGHT or self.game_object_proxy.get_object_type(possible_tile) is GameObjectType.MAGICIAN:
                cnt += 1
        return cnt

    def base_neighbours(self):
        neighbours = self.map_proxy.get_bases_positions().pop().get_all_neighbours()
        visible_tiles = self.map_proxy.get_player_visible_tiles()
        res = set()
        for neighbour in neighbours:
            if not self.map_proxy.is_position_occupied(neighbour) and neighbour in visible_tiles:
                res.add(neighbour)
        #print("All free neighbours of the base")
        #for item in res:
        #    print(item.offset.q, item.offset.r)
        return res

    def position_first_round(self):
        if not self.map_proxy.player_have_base(self.player):
            return None
        positions_first_round = self.list_positions_first_round()
        visible_tiles = self.map_proxy.get_player_visible_tiles()
        border_tiles = self.map_proxy.get_border_tiles()
        for pos in positions_first_round:
            if pos in visible_tiles and not self.map_proxy.is_position_occupied(pos) and self.map_proxy.is_position_on_map(pos) and pos not in border_tiles:
                return pos
            return None


    def position_second_round(self):
        if not self.map_proxy.player_have_base(self.player):
            return None
        positions_second_round = self.list_positions_second_round()
        visible_tiles = self.map_proxy.get_player_visible_tiles()
        border_tiles = self.map_proxy.get_border_tiles()
        for pos in positions_second_round:
            if pos in visible_tiles and not self.map_proxy.is_position_occupied(pos) and self.map_proxy.is_position_on_map(pos) and pos not in border_tiles:
                return pos
        return None

    def position_third_round(self):
        if not self.map_proxy.player_have_base(self.player):
            return None
        visible_tiles = self.map_proxy.get_player_visible_tiles()
        border_tiles = self.map_proxy.get_border_tiles()
        positions_third_round = self.list_positions_third_round()
        for pos in positions_third_round:
            if pos in visible_tiles and not self.map_proxy.is_position_occupied(pos) and self.map_proxy.is_position_on_map(pos) and pos not in border_tiles:
                return pos
        return None


    def positions(self):
        if not self.map_proxy.player_have_base(self.player):
            return None
        res = []
        res.extend(self.list_positions_first_round())
        res.extend(self.list_positions_second_round())
        res.extend(self.list_positions_third_round())
        return res



    def list_positions_first_round(self):
        if not self.map_proxy.player_have_base(self.player):
            return None
        base_position = self.map_proxy.get_bases_positions().pop()
        positions_first_round = set()
        positions_first_round.add(OffsetPosition(base_position.offset.q,     base_position.offset.r - 1))
        positions_first_round.add(OffsetPosition(base_position.offset.q + 1, base_position.offset.r    ))
        positions_first_round.add(OffsetPosition(base_position.offset.q + 1, base_position.offset.r + 1))
        positions_first_round.add(OffsetPosition(base_position.offset.q,     base_position.offset.r + 1))
        positions_first_round.add(OffsetPosition(base_position.offset.q - 1, base_position.offset.r + 1))
        positions_first_round.add(OffsetPosition(base_position.offset.q - 1, base_position.offset.r    ))
        return positions_first_round



    def list_positions_second_round(self):
        if not self.map_proxy.player_have_base(self.player):
            return None
        base_position = self.map_proxy.get_bases_positions().pop()
        positions_second_round = set()
        positions_second_round.add(OffsetPosition(base_position.offset.q,     base_position.offset.r - 2))
        positions_second_round.add(OffsetPosition(base_position.offset.q + 1, base_position.offset.r - 2))
        positions_second_round.add(OffsetPosition(base_position.offset.q + 2, base_position.offset.r - 1))
        positions_second_round.add(OffsetPosition(base_position.offset.q + 2, base_position.offset.r    ))
        positions_second_round.add(OffsetPosition(base_position.offset.q + 2, base_position.offset.r + 1))
        positions_second_round.add(OffsetPosition(base_position.offset.q + 1, base_position.offset.r + 1))
        positions_second_round.add(OffsetPosition(base_position.offset.q,     base_position.offset.r + 2))
        positions_second_round.add(OffsetPosition(base_position.offset.q - 1, base_position.offset.r + 1))
        positions_second_round.add(OffsetPosition(base_position.offset.q - 2, base_position.offset.r + 1))
        positions_second_round.add(OffsetPosition(base_position.offset.q - 2, base_position.offset.r    ))
        positions_second_round.add(OffsetPosition(base_position.offset.q - 2, base_position.offset.r - 1))
        positions_second_round.add(OffsetPosition(base_position.offset.q - 1, base_position.offset.r - 2))
        return positions_second_round


    def list_positions_third_round(self):
        if not self.map_proxy.player_have_base(self.player):
            return None
        base_position = self.map_proxy.get_bases_positions().pop()
        positions_third_round = set()
        positions_third_round.add(OffsetPosition(base_position.offset.q,     base_position.offset.r - 3))
        positions_third_round.add(OffsetPosition(base_position.offset.q + 1, base_position.offset.r - 2))
        positions_third_round.add(OffsetPosition(base_position.offset.q + 2, base_position.offset.r - 2))
        positions_third_round.add(OffsetPosition(base_position.offset.q + 3, base_position.offset.r - 1))
        positions_third_round.add(OffsetPosition(base_position.offset.q + 3, base_position.offset.r    ))
        positions_third_round.add(OffsetPosition(base_position.offset.q + 3, base_position.offset.r + 1))
        positions_third_round.add(OffsetPosition(base_position.offset.q + 3, base_position.offset.r + 2))
        positions_third_round.add(OffsetPosition(base_position.offset.q + 2, base_position.offset.r + 2))
        positions_third_round.add(OffsetPosition(base_position.offset.q + 1, base_position.offset.r + 3))
        positions_third_round.add(OffsetPosition(base_position.offset.q,     base_position.offset.r + 3))
        positions_third_round.add(OffsetPosition(base_position.offset.q - 1, base_position.offset.r + 3))
        positions_third_round.add(OffsetPosition(base_position.offset.q - 2, base_position.offset.r + 2))
        positions_third_round.add(OffsetPosition(base_position.offset.q - 3, base_position.offset.r + 2))
        positions_third_round.add(OffsetPosition(base_position.offset.q - 3, base_position.offset.r + 1))
        positions_third_round.add(OffsetPosition(base_position.offset.q - 3, base_position.offset.r    ))
        positions_third_round.add(OffsetPosition(base_position.offset.q - 3, base_position.offset.r - 1))
        positions_third_round.add(OffsetPosition(base_position.offset.q - 2, base_position.offset.r - 2))
        positions_third_round.add(OffsetPosition(base_position.offset.q - 1, base_position.offset.r - 2))
        return positions_third_round

    def first_free_tile(self, terrain_type: str):
        """ Find random tile with given terrain type """
        tiles = self.map_proxy.get_inner_tiles()
        border_tiles = self.map_proxy.get_border_tiles()

        for position in tiles:
            terrain = self.map_proxy.get_terrain_type(position) == TerrainType.from_string(terrain_type)
            if terrain and position not in border_tiles:
                return position
        return None

    def visible_free_tile(self, terrain_type: str):
        """ Find random free tile with given terrain type """
        tiles = self.map_proxy.get_player_visible_tiles()
        border_tiles = self.map_proxy.get_border_tiles()

        for position in tiles:
            terrain = self.map_proxy.get_terrain_type(position) == TerrainType.from_string(terrain_type)
            occupied = self.map_proxy.is_position_occupied(position)
            if terrain and not occupied and position not in border_tiles:
                return position
        return None

