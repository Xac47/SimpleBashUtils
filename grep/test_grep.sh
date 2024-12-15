#!/bin/bash

# Создание временных файлов для тестов
echo -e "One\nTwo\nthree\nfour\nFIVE\nsix\nseven\nEight\nNine\nten" > test_grep1.txt
echo -e "One\nthree\nFive\nseven\nNine" > test_grep2.txt
echo -e "One\nTwo\nThree\nFour\nFive\nSix\nSeven\nEight\nNine\nTen" > test_grep3.txt

# Определение файлов и опций для тестов
FILES=("test_grep1.txt" "test_grep2.txt" "test_grep3.txt")
OPTIONS=("-e One" "-e Two" "-e three" "-e Four" "-e five" "-e Six" "-e Seven" "-e Eight" "-e Nine" "-e ten")
OPTIONS2=("-i" "-v" "-c" "-l" "-n" "-h" "-s")
OPTIONS3=("-iv" "-ic" "-ivc" "-in" "-h" "-s")

# Функция для выполнения одного теста
run_test() {
    local option=$1
    local option2=$2
    local option3=$3
    local file=$4
    local file2=$5

    ../grep/s21_grep $option $option2 $option3 $file $file2 > test.out
    grep $option $option2 $option3 $file $file2 > ref.out
    diff ref.out test.out -q

    if [ $? -eq 0 ]; then
        echo "PASS: s21_grep $option $option2 $option3 $file $file2"
        return 0
    else
        echo "FAIL: s21_grep $option $option2 $option3 $file $file2"
        return 1
    fi
}

# Переменные для отчета
total_tests=0
passed_tests=0
failed_tests=0

# Выполнение всех тестов
for file in "${FILES[@]}"; do
    for file2 in "${FILES[@]}"; do
        for option in "${OPTIONS[@]}"; do
            for option2 in "${OPTIONS2[@]}"; do
                for option3 in "${OPTIONS3[@]}"; do
                    total_tests=$((total_tests + 1))
                    if run_test "$option" "$option2" "$option3" "$file" "$file2"; then
                        passed_tests=$((passed_tests + 1))
                    else
                        failed_tests=$((failed_tests + 1))
                    fi
                done
            done
        done
    done
done

# Отчет по результатам тестирования
echo
echo "Total tests: $total_tests"
echo "Passed tests: $passed_tests"
echo "Failed tests: $failed_tests"

# Удаление временных файлов
rm test_grep1.txt test_grep2.txt test_grep3.txt test.out ref.out

# Выход с кодом завершения, равным числу неудачных тестов
exit $failed_tests