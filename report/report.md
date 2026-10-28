**Цель работы:** Управление потоками в ОС, обеспечение синхронизации между потоками.

**Задание:** составить программу на языке Си, обрабатывающую данные в многопоточном режиме, используя стандартные средства создания потоков операционной системы.

**Вариант:** №15. Есть колода из 52 карт, рассчитать экспериментально вероятность того, что сверху лежат 2 одинаковые карты (Одинаковыми картами будут считаться карты одинакового ранга)

**Общее описание решения:**
В качестве системы для разработки был выбран Linux (EndeavourOS).
Процессор – Intel Core Ultra 5 226v (8 ядер, 8 потоков).
Проект состоит из 2 файлов:
1.    Main.с – точка входа, функции для симуляций.
2.    CMakeLists.txt – файл сборки Cmake. Собирает 2 исполняемых файла: program и child.

**Ключевые фрагменты кода:**
Создание потоков, передача данных в потоки:

``` C
for(unsigned int i = 0; i < numOfThreads; ++i){

        SimulationParameters* parameters = (SimulationParameters*)malloc(sizeof(SimulationParameters));

        parameters->seed = rand();

        parameters->numOfSimulations = simulationsPerThread;

        if(pthread_create(&threads[i], NULL, Simulate, parameters) != 0){

            perror("Thread error\n");

            return 1;

        }

    }
```


Ожидание завершения потоков, сбор данных:

```C
for(unsigned int i = 0; i < numOfThreads; ++i){

        void* threadResult = NULL;

        if(pthread_join(threads[i], &threadResult) != 0){

            perror("Thread join error\n");

            return 1;

        }

        succesfulSimulations += *(unsigned int*)threadResult;

        free(threadResult);

    }

```


Многопоточная функция:
```C
void* Simulate(void* argument){

    SimulationParameters parameters = *(SimulationParameters*)argument;

    free(argument);

    unsigned int succesfulSimulations = 0;

    for(unsigned int i = 0; i < parameters.numOfSimulations; ++i){

        succesfulSimulations += AreSameRanksOnTop(&parameters.seed);

    }

    unsigned int* result = (unsigned int*)malloc(sizeof(unsigned int));

    *result = succesfulSimulations;

    return (void*)result;

}
```


Симуляция:
```C
int AreSameRanksOnTop(unsigned int* seed){

    Card cards[52];

    for(unsigned int suit = 0; suit < 4; ++suit){

        for(unsigned int rank = 0; rank < 13; ++rank){

            unsigned int index = suit * 13 + rank;

            cards[index].suit = suit;

            cards[index].rank = rank;

        }

    }

    for(unsigned int i = 52; i > 0;){

        --i;

        unsigned int swapIndex = rand_r(seed) % (i + 1);

        Swap(&cards[i], &cards[swapIndex]);

    }

    return cards[0].rank == cards[1].rank;

}
```


**Запуск программы:**

Проведены замеры зависимости реального времени выполнения от количества потоков. Тесты проводились для 100000000 симуляций. Эффективный рост производительности наблюдается в районе 1-5 потоков.

Составлена таблица:

| Потоки | Реальное время выполнения в секундах |
| ------ | ------------------------------------ |
| 1      | 46.412                               |
| 2      | 23.481                               |
| 3      | 15.836                               |
| 4      | 12.248                               |
| 5      | 9.598                                |
| 6      | 10.373                               |
| 7      | 8.658                                |
| 8      | 8.555                                |
| 10     | 7.819                                |
| 20     | 7.206                                |
| 30     | 7.863                                |
| 40     | 7.152                                |

**Вывод:**

