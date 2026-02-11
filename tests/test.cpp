#include "skip_list.hpp"
#include <iostream>
#include <string>
#include <cassert>

void demonstrateIntSkipList() {
    std::cout << "\n=== Целочисленный скип-лист ===\n";
    SkipList<int> list(0.5);

    // Вставка
    for (int x : {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5}) {
        list.insert(x);
    }

    list.printByLevels();

    // Поиск
    assert(list.contains(5) == true);
    assert(list.contains(7) == false);
    std::cout << "Поиск 5: " << (list.contains(5) ? "найден" : "нет") << '\n';
    std::cout << "Поиск 7: " << (list.contains(7) ? "найден" : "нет") << '\n';

    // Удаление
    list.erase(5);
    list.erase(1);
    list.erase(10); // не существует
    std::cout << "После удаления 5 и 1:\n";
    list.printByLevels();

    // Итерация
    std::cout << "Все элементы по порядку: ";
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << *it << ' ';
    }
    std::cout << '\n';
}

void demonstrateStringSkipList() {
    std::cout << "\n=== Строковый скип-лист ===\n";
    SkipList<std::string> list(0.25); // меньшая вероятность

    list.insert("apple");
    list.insert("banana");
    list.insert("cherry");
    list.insert("date");
    list.insert("fig");

    list.printByLevels();

    std::cout << "Содержит 'banana'? " << std::boolalpha << list.contains("banana") << '\n';
    std::cout << "Содержит 'grape'? " << list.contains("grape") << '\n';

    list.erase("banana");
    std::cout << "После удаления 'banana':\n";
    for (const auto& fruit : list) {
        std::cout << fruit << ' ';
    }
    std::cout << '\n';
}

void demonstrateMoveSemantics() {
    std::cout << "\n=== Перемещение ===\n";
    SkipList<int> list1;
    list1.insert(10);
    list1.insert(20);

    SkipList<int> list2 = std::move(list1); // перемещающий конструктор
    std::cout << "list2 после перемещения:\n";
    list2.printByLevels();

    SkipList<int> list3;
    list3 = std::move(list2); // перемещающее присваивание
    std::cout << "list3 после присваивания:\n";
    list3.printByLevels();
}

int main() {
    demonstrateIntSkipList();
    demonstrateStringSkipList();
    demonstrateMoveSemantics();

    std::cout << "\nВсе тесты пройдены успешно.\n";
    return 0;
}
