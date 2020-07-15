from typing import List
from OrodaelTurrim.Business.Interface.Player import PlayerTag
from OrodaelTurrim.Business.Proxy import MapProxy, GameObjectProxy, GameUncertaintyProxy
from ExpertSystem.Business.UserFramework import IKnowledgeBase
from ExpertSystem.Structure.RuleBase import Fact
from OrodaelTurrim.Structure.Enums import TerrainType, AttributeType, EffectType, GameRole
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


    def ideal_tile_probability(self):
        if not self.map_proxy.player_have_base(self.player):
            return 0
        spawn_info = self.game_uncertainty_proxy.spawn_information()

        next_round = spawn_info[0]

        chances_global = {'N': 0, 'S': 0, 'W':0, 'E':0 }

        possible_tiles = set()

        for unit in next_round:
            uncertainty = unit.positions[0].uncertainty
            N = S = E = W = False
            for tile in unit.positions:  # tiles where the unit can be spawned
                pos = tile.position.offset
                if pos.r == -6:
                    W = True
                if pos.r == 6:
                    E = True
                if pos.q == -6:
                    N = True
                if pos.q == 6:
                    S = True
            possible_tiles.update([x.position for x in unit.positions])
            if W:
                chances_global['W'] += uncertainty
            if E:
                chances_global['E'] += uncertainty
            if N:
                chances_global['N'] += uncertainty
            if S:
                chances_global['S'] += uncertainty

            # normalisation
            maximum = max(chances_global.values())
            suma = sum(chances_global.values())
            if maximum:
                for key in chances_global:
                    chances_global[key] /= suma
            #print( "Ideal tile probability returning", max(chances_global.values()))
            return max(chances_global.values())



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
        facts.append(Fact('base_neighbours', eval_function=self.base_neighbours, data=self.base_neighbours))
        facts.append(Fact('ideal_tile', eval_function=self.ideal_tile, data=self.ideal_tile, probability=self.ideal_tile_probability()))
        facts.append(Fact('position_first_round', eval_function=self.position_first_round, data=self.position_first_round))
        facts.append(Fact('position_second_round', eval_function=self.position_second_round, data=self.position_second_round))
        facts.append(Fact('position_third_round', eval_function=self.position_third_round, data=self.position_third_round))

        # Add numerical fact
        facts.append(Fact("money", lambda: self.game_object_proxy.get_resources(self.player)))
        facts.append(Fact('current_round', lambda: self.game_object_proxy.get_current_round()))

        # print("money:", self.game_object_proxy.get_resources(self.player))

        return facts

    def player_has_base(self):
        return self.map_proxy.player_have_base(self.player)

    def player_dont_have_base(self): # Czenglish, I know
        return not self.map_proxy.player_have_base(self.player)


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




    def appropriate_positions_first_round(self, coordinate):
        if not self.map_proxy.player_have_base(self.player):
            return []
        base_position = self.map_proxy.get_bases_positions().pop()
        all_positions = self.base_neighbours()
        visible_positions = self.map_proxy.get_player_visible_tiles()
        border_positions = self.map_proxy.get_border_tiles()

        res = []
        if coordinate == 'N':
            pos = OffsetPosition(int(base_position.offset.q), int(base_position.offset.r-1))
            if pos in all_positions and pos in visible_positions and pos not in border_positions:
                res.append(pos)
        elif coordinate == 'S':
            pos = OffsetPosition(int(base_position.offset.q), int(base_position.offset.r+1))
            if pos in all_positions and pos in visible_positions and pos not in border_positions:
                res.append(pos)
        elif coordinate == 'W':
            pos = [OffsetPosition(int(base_position.offset.q-1), int(base_position.offset.r-1)),
                   OffsetPosition(int(base_position.offset.q-1), int(base_position.offset.r+1))]
            for p in pos:
                if p in all_positions and p in visible_positions and p not in border_positions:
                    res.append(p)
        else:
            pos = [OffsetPosition(int(base_position.offset.q+1), int(base_position.offset.r-1)),
                   OffsetPosition(int(base_position.offset.q+1), int(base_position.offset.r+1))]
            for p in pos:
                if p in all_positions and p in visible_positions and p not in border_positions:
                    res.append(p)
        return res

    def appropriate_positions_second_round(self, coordinate):
        pass


    def appropriate_positions_third_round(self, coordinate):
        pass

    def ideal_tile(self):
        if not self.map_proxy.player_have_base(self.player):
            return 0
        spawn_info = self.game_uncertainty_proxy.spawn_information()

        next_round = spawn_info[0]

        chances_global = {'N': 0, 'S': 0, 'W':0, 'E':0 }

        possible_tiles = set()

        for unit in next_round:
            uncertainty = unit.positions[0].uncertainty
            N = S = E = W = False
            for tile in unit.positions:  # tiles where the unit can be spawned
                pos = tile.position.offset
                if pos.r == -6:
                    W = True
                if pos.r == 6:
                    E = True
                if pos.q == -6:
                    N = True
                if pos.q == 6:
                    S = True
            possible_tiles.update([x.position for x in unit.positions])
            if W:
                chances_global['W'] += uncertainty
            if E:
                chances_global['E'] += uncertainty
            if N:
                chances_global['N'] += uncertainty
            if S:
                chances_global['S'] += uncertainty

            # normalisation
            maximum = max(chances_global.values())
            if maximum:
                for key in chances_global:
                    chances_global[key] /= maximum

        pos = []
        if max(chances_global.values()) == chances_global['N']:
            pos.extend(self.appropriate_positions_first_round('N'))
        elif max(chances_global.values()) == chances_global['S']:
            pos.extend(self.appropriate_positions_first_round('S'))
        elif max(chances_global.values()) == chances_global['W']:
            pos.extend(self.appropriate_positions_first_round('W'))
        else:
            pos.extend(self.appropriate_positions_first_round('E'))

        if pos:
            #print("Ideal_tile returning:", pos[0])
            return pos[0]
        return None

    def list_positions_first_round(self):
        if not self.map_proxy.player_have_base(self.player):
            return None
        base_position = self.map_proxy.get_bases_positions().pop()
        positions_first_round = []
        positions_first_round.append(OffsetPosition(base_position.offset.q,     base_position.offset.r - 1))
        positions_first_round.append(OffsetPosition(base_position.offset.q + 1, base_position.offset.r    ))
        positions_first_round.append(OffsetPosition(base_position.offset.q + 1, base_position.offset.r + 1))
        positions_first_round.append(OffsetPosition(base_position.offset.q,     base_position.offset.r + 1))
        positions_first_round.append(OffsetPosition(base_position.offset.q - 1, base_position.offset.r + 1))
        positions_first_round.append(OffsetPosition(base_position.offset.q - 1, base_position.offset.r    ))
        return positions_first_round



    def list_positions_second_round(self):
        if not self.map_proxy.player_have_base(self.player):
            return None
        base_position = self.map_proxy.get_bases_positions().pop()
        positions_second_round = []
        positions_second_round.append(OffsetPosition(base_position.offset.q,     base_position.offset.r - 2))
        positions_second_round.append(OffsetPosition(base_position.offset.q + 1, base_position.offset.r - 2))
        positions_second_round.append(OffsetPosition(base_position.offset.q + 2, base_position.offset.r - 1))
        positions_second_round.append(OffsetPosition(base_position.offset.q + 2, base_position.offset.r    ))
        positions_second_round.append(OffsetPosition(base_position.offset.q + 2, base_position.offset.r + 1))
        positions_second_round.append(OffsetPosition(base_position.offset.q + 1, base_position.offset.r + 1))
        positions_second_round.append(OffsetPosition(base_position.offset.q,     base_position.offset.r + 2))
        positions_second_round.append(OffsetPosition(base_position.offset.q - 1, base_position.offset.r + 1))
        positions_second_round.append(OffsetPosition(base_position.offset.q - 2, base_position.offset.r + 1))
        positions_second_round.append(OffsetPosition(base_position.offset.q - 2, base_position.offset.r    ))
        positions_second_round.append(OffsetPosition(base_position.offset.q - 2, base_position.offset.r - 1))
        positions_second_round.append(OffsetPosition(base_position.offset.q - 1, base_position.offset.r - 2))
        return positions_second_round


    def list_positions_third_round(self):
        if not self.map_proxy.player_have_base(self.player):
            return None
        base_position = self.map_proxy.get_bases_positions().pop()
        positions_third_round = []
        positions_third_round.append(OffsetPosition(base_position.offset.q,     base_position.offset.r - 3))
        positions_third_round.append(OffsetPosition(base_position.offset.q + 1, base_position.offset.r - 2))
        positions_third_round.append(OffsetPosition(base_position.offset.q + 2, base_position.offset.r - 2))
        positions_third_round.append(OffsetPosition(base_position.offset.q + 3, base_position.offset.r - 1))
        positions_third_round.append(OffsetPosition(base_position.offset.q + 3, base_position.offset.r    ))
        positions_third_round.append(OffsetPosition(base_position.offset.q + 3, base_position.offset.r + 1))
        positions_third_round.append(OffsetPosition(base_position.offset.q + 3, base_position.offset.r + 2))
        positions_third_round.append(OffsetPosition(base_position.offset.q + 2, base_position.offset.r + 2))
        positions_third_round.append(OffsetPosition(base_position.offset.q + 1, base_position.offset.r + 3))
        positions_third_round.append(OffsetPosition(base_position.offset.q,     base_position.offset.r + 3))
        positions_third_round.append(OffsetPosition(base_position.offset.q - 1, base_position.offset.r + 3))
        positions_third_round.append(OffsetPosition(base_position.offset.q - 2, base_position.offset.r + 2))
        positions_third_round.append(OffsetPosition(base_position.offset.q - 3, base_position.offset.r + 2))
        positions_third_round.append(OffsetPosition(base_position.offset.q - 3, base_position.offset.r + 1))
        positions_third_round.append(OffsetPosition(base_position.offset.q - 3, base_position.offset.r    ))
        positions_third_round.append(OffsetPosition(base_position.offset.q - 3, base_position.offset.r - 1))
        positions_third_round.append(OffsetPosition(base_position.offset.q - 2, base_position.offset.r - 2))
        positions_third_round.append(OffsetPosition(base_position.offset.q - 1, base_position.offset.r - 2))
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

