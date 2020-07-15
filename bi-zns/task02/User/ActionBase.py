from OrodaelTurrim.Business.Interface.Player import PlayerTag
from OrodaelTurrim.Business.Proxy import GameControlProxy
from ExpertSystem.Business.UserFramework import IActionBase
from OrodaelTurrim.Business.Logger import Logger
from OrodaelTurrim.Structure.Enums import GameObjectType
from OrodaelTurrim.Structure.Filter.AttackFilter import AttackStrongestFilter
from OrodaelTurrim.Structure.Filter.Factory import FilterFactory
from OrodaelTurrim.Structure.GameObjects.GameObject import SpawnInformation
from OrodaelTurrim.Structure.Position import OffsetPosition

from User.AttackFilter import DummyAttackFilter, EmptyAttackFilter


class ActionBase(IActionBase):
    """
    You can define here your custom actions. Methods must be public (not starting with __ or _) and must have unique
    names. Methods could have as many arguments as you want. Instance of this class will be available in
    Inference class.

    **This class provides:**

    * self.game_control_proxy [GameControlProxy] for doing actions in game
    * self.player [PlayerTag] instance of your player for identification yourself in proxy

    Usage of ActionBase is described in documentation.


    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    !!               TODO: Write implementation of your actions HERE                !!
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    """
    game_control_proxy: GameControlProxy
    player: PlayerTag


    def build_base(self, q, r):
        # Custom log messages
        Logger.log('Building base')

        # Create instance of custom filter
        empty_filter = FilterFactory().attack_filter(EmptyAttackFilter)
        dummy_filter = FilterFactory().attack_filter(DummyAttackFilter, 'Base attacking')

        # Create instance of default filter
        strongest_filter = FilterFactory().attack_filter(AttackStrongestFilter)

        self.game_control_proxy.spawn_unit(
            SpawnInformation(self.player,
                             GameObjectType.BASE,
                             OffsetPosition(int(q), int(r)),
                             [empty_filter, dummy_filter, strongest_filter], []))

    def build_archer(self, **kwargs):
        pos = None
        for key in ('ideal_tile', 'position_first_round', 'position_second_round', 'position_third_round'):
            if key in kwargs:
                pos = kwargs[key]
        Logger.log('Building archer')
        self.game_control_proxy.spawn_unit(
            SpawnInformation(self.player,
                             GameObjectType.ARCHER,
                             pos,
                             [], []))

    def build_knight(self, **kwargs):
        pos = None
        for key in ('ideal_tile', 'position_first_round', 'position_second_round', 'position_third_round'):
            if key in kwargs:
                pos = kwargs[key]
        Logger.log('Building knight')
        self.game_control_proxy.spawn_unit(
            SpawnInformation(self.player,
                             GameObjectType.KNIGHT,
                             pos,
                             [], []))

    def build_magician(self, **kwargs):
        pos = None
        for key in ('ideal_tile', 'position_first_round', 'position_second_round', 'position_third_round'):
            if key in kwargs:
                pos = kwargs[key]
        Logger.log('Building magician')
        self.game_control_proxy.spawn_unit(
            SpawnInformation(self.player,
                             GameObjectType.MAGICIAN,
                             pos,
                             [], []))

    def build_ent(self, **kwargs):
        pos = None
        for key in ('ideal_tile', 'position_first_round', 'position_second_round', 'position_third_round'):
            if key in kwargs:
                pos = kwargs[key]
        Logger.log('Building ent')
        self.game_control_proxy.spawn_unit(
            SpawnInformation(self.player,
                             GameObjectType.ENT,
                             pos,
                             [], []))

    def build_druid(self, **kwargs):
        pos = None
        for key in ('ideal_tile', 'position_first_round', 'position_second_round', 'position_third_round'):
            if key in kwargs:
                pos = kwargs[key]
        Logger.log('Building druid')
        self.game_control_proxy.spawn_unit(
            SpawnInformation(self.player,
                             GameObjectType.DRUID,
                             pos,
                             [], []))



