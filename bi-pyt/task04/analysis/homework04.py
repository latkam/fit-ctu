import pandas as pd
import numpy as np

"""
Dataset obsahuje nasledujici promenne:
 'Age' - vek v rocich
 'Fare' - cena jizdenky
 'Name' - jmeno cestujiciho
 'Parch' - pocet rodicu/deti daneho cloveka na palube
 'PassengerId' - Id
 'Pclass' - Trida, 1 = 1. trida, 2 = 2.trida, 3 = 3.trida
 'Sex' - pohlavi
 'SibSp' - # sourozencu/manzelu daneho cloveka na ppalube
 'Survived' - 0 = Neprezil, 1 = Prezil
 'Embarked' - Pristav, kde se dany clovek nalodil. C = Cherbourg, Q = Queenstown, S = Southampton
 'Cabin' - Cislo kabiny
 'Ticket' - Cislo tiketu
"""



def load_dataset(train_file_path, test_file_path):
    """
    Napiste funkci, ktera nacte soubory ze souboru zadanych parametrem a vytvori dva separatni DataFrame. Pro testovani
    vyuzijte data 'data/train.csv' a 'data/test.csv'
    Ke kazdemu dataframe pridejte sloupecek pojmenovaný jako "Label", ktery bude obsahovat hodnoty "Train" pro train.csv
    a "Test" pro test.csv.

    1. Pote slucte oba dataframy.
    2. Z vysledneho slouceneho DataFramu odstraňte sloupce  "Ticket", "Embarked", "Cabin".
    3. Sloučený DataDrame bude mít index od 0 do do počtu řádků.
    4. Vratte slouceny DataDrame.
    """

    df_train = pd.read_csv(train_file_path)
    df_test = pd.read_csv(test_file_path)

    df_train['Label'] = "Train"
    df_test['Label'] = "Test"

    df = (
        pd.concat([df_train, df_test], sort=True)
        .drop(['Ticket', 'Embarked', 'Cabin'], axis="columns")
    )

    df[''] = list(range(len(df)))
    df.set_index('', inplace=True)

    return df


def get_missing_values(df : pd.DataFrame) -> pd.DataFrame:
    """
    Ze zadaneho dataframu zjistete chybejici hodnoty. Vyvorte DataFrame, ktery bude obsahovat v indexu jednotlive
    promenne a ve prvnim sloupci bude promenna 'Total' obsahujici celkovy pocet chybejicich hodnot a ve druhem sloupci
    promenna 'Percent', ve ktere bude procentualni vyjadreni chybejicich hodnot vuci celkovemu poctu radku v tabulce.
    DataFrame seradte od nejvetsich po nejmensi hodnoty.
    Vrattre DataFrame chybejicich hodnot a celkovy pocet chybejicich hodnot.

    Priklad:

               |  Total  |  Percent
    "Column1"  |   34    |    76
    "Column2"  |   0     |    0

    """
    missing = pd.DataFrame(columns=['', 'Total', 'Percent'])
    missing[''] = list(df.columns)
    missing.set_index('', inplace=True)
    missing['Total'] = list(df.isna().sum())
    missing['Percent'] = missing['Total'] / len(df) * 100
    missing.sort_values('Percent', ascending=False, inplace=True)
    return missing


def substitute_missing_values(df: pd.DataFrame) -> pd.DataFrame:
    """
    Chybejici hodnoty ve sloupecku "Age" nahradte meanem hodnot z "Age".
    Chybejici hodnoty ve sloupecku "Fare" nahradte meadianem hodnot z "Fare".
    V jednom pripade pouzijte "loc" a ve druhem "fillna".
    Zadany DataFrame neupravujte, ale vytvorte si kopii.
    Vratte upraveny DataFrame.
    """
    df_c = df.copy()
    df_c.loc[np.isnan(df_c['Age']), 'Age'] = df_c['Age'].mean()

    df_c['Fare'] = df_c['Fare'].fillna(df_c['Fare'].median())

    return df_c


def get_correlation(df: pd.DataFrame) -> float:
    """
    Spocitejte korelaci pro "Age" a "Fare" a vratte korelaci mezi "Age" a "Fare".
    """

    return df['Age'].corr(df['Fare'])


def get_survived_per_class(df : pd.DataFrame, group_by_column_name : str) ->pd.DataFrame:
    """
    Spocitejte prumer z promenne "Survived" pro kazdou skupinu zadanou parametrem "group_by_column_name".
    Hodnoty seradte od nejvetsich po mejmensi.
    Hodnoty "Survived" zaokhroulete na 2 desetinna mista.
    Vratte pd.DataFrame.

    Priklad:

    get_survived_per_class(df, "Sex")

                 Survived
    Male     |      0.32
    Female   |      0.82

    """

    df['tmp'] = 0
    df = df.groupby([group_by_column_name])['tmp', 'Survived'].mean().sort_values(by='Survived', ascending=False).round(2)
    df.reset_index(drop=False, inplace=True)
    df.drop(columns='tmp', inplace=True)
    return df


def get_outliers(df: pd.DataFrame) -> (int, str):
    """
    Vyfiltrujte odlehle hodnoty (outliers) ve sloupecku "Fare" pomoci metody IRQ.
    Tedy spocitejte rozdil 3. a 1. kvantilu, tj. IQR = Q3 - Q1.
    Pote odfiltrujte vsechny hodnoty nesplnujici: Q1 - 1.5*IQR < "Fare" < Q3 + 1.5*IQR.
    Namalujte box plot pro sloupec "Fare" pred a po vyfiltrovani outlieru.
    Vratte tuple obsahujici pocet outlieru a jmeno cestujiciho pro nejvetsi outlier.
    """

    df.sort_values('Fare', inplace=True)

    #boxplot1 = df.boxplot(column='Fare')

    q1 = df['Fare'].quantile(0.25)
    q3 = df['Fare'].quantile(0.75)
    iqr = q3 - q1
    lower_bound = q1 - 1.5 * iqr
    upper_bound = q3 + 1.5 * iqr

    outliers1 = df[df['Fare'] < lower_bound]
    outliers2 = df[df['Fare'] > upper_bound]
    outliers = pd.concat([outliers1, outliers2])
    outliers.sort_values(by=['Fare', 'Name'], inplace=True)

    #boxplot2 = inliers.boxplot(column='Fare')

    return len(outliers), [x for x in outliers.tail(1)['Name'].values][0]


def normalise(df: pd.DataFrame, col: str) -> pd.DataFrame:
    """
    Naskalujte sloupec "col" zadany parametrem pro kazdou "Pclass" hodnotu z dataframu "df" zvlast.
    Pouzijte vzorec: scaled_x_i = (x_i - min(x)) / (max(x) - min(x)), kde "x_i" prestavuje konkretni hodnotu ve sloupeci "col".
    Vratte naskalovany dataframe.
    """

    normalised = df.copy()
    group = df.groupby("Pclass")
    scaled = group.apply(lambda x: (x[col] - x[col].min())/(x[col].max() - x[col].min()))
    scaled = scaled.reset_index().sort_values('').set_index('')
    normalised[col] = scaled[col]
    return normalised


def create_new_features(df: pd.DataFrame) -> pd.DataFrame:
    """
    Vytvorte 3 nove promenne:
    1. "Fare_scaled" - vytvorte z "Fare" tak, aby mela nulovy prumer a jednotkovou standartni odchylku.
    2. "Age_log" - vytvorte z "Age" tak, aby nova promenna byla logaritmem puvodni "Age".
    3. "Sex" - Sloupec "Sex" nahradte: "female" -> 1, "male" -> 0, kde 0 a 1 jsou integery.

    Nemodifikujte predany DataFrame, ale vytvorte si novy, upravte ho a vratte jej.
    """
    df_c = df.copy()
    df_c['Fare_scaled'] = (df_c['Fare'] - df_c['Fare'].mean())/df_c['Fare'].std(axis=0, skipna=True)
    df_c['Age_log'] = np.log(df['Age'])
    df_c['Sex'].replace('female', 1, inplace=True)
    df_c['Sex'].replace('male', 0, inplace=True)
    return df_c


def determine_survival(df: pd.DataFrame, n_interval: int, age: float, sex: str) -> float:
    """
    Na zaklade statistickeho zpracovani dat zjistete pravdepodobnost preziti Vami zadaneho cloveka (zadava se vek a pohlavi pomoci parametru "age" a "sex")

    Vsechny chybejici hodnoty ve vstupnim DataFramu ve sloupci "Age" nahradte prumerem.
    Rozdelte "Age" do n intervalu zadanych parametrem "n_interval". Napr. pokud bude Age mit hodnoty [2, 13, 18, 25] a mame jej rozdelit do 2 intervalu,
    tak bude vysledek:

    0    (1.977, 13.5]
    1    (1.977, 13.5]
    2     (13.5, 25.0]
    3     (13.5, 25.0]

    Pridejte k rozdeleni jeste pohlavi. Tj. pro kazdou kombinaci pohlavi a intervalu veku zjistete prumernou
    pravdepodobnost preziti ze sloupce "Survival" a tu i vratte.

    Vysledny DataFrame:

    "AgeInterval"   |    "Sex"    |   "Survival Probability"
       (0-10)       | "male"      |            0.21
       (0-10)       | "female"    |            0.28
       (10-20)      | "male"      |            0.10
       (10-20)      | "female"    |            0.15
       atd...

    Takze vystup funkce determine_survival(df, n_interval=20, age = 5, sex = "male") bude 0.21. Tato hodnota bude navratovou hodnotou funkce.

    """

    df['Age'] = df['Age'].fillna(df['Age'].dropna().mean())

    if age < df['Age'].min() or age > df['Age'].max():
        return 0

    res = pd.DataFrame(columns=['AgeInterval', 'Left', 'Right'])
    intervals = pd.cut(df['Age'], n_interval, retbins=False).unique()

    res['AgeInterval'] = pd.Series(intervals)
    res['Left'] = pd.Series([interval.left for interval in intervals])
    res['Right'] = pd.Series([interval.right for interval in intervals])
    res.sort_values('AgeInterval', inplace=True)

    data = res[(res['Left'] < age) & (res['Right'] > age)]
    left = pd.to_numeric(data.loc[:, 'Left'].values[0])
    right = pd.to_numeric(data.loc[:, 'Right'].values[0])
    return df[(df['Age'] > left) & (df['Age'] <= right) & (df['Sex'] == sex)]['Survived'].mean()



