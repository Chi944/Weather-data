#ifndef MENU_HANDLER_H_INCLUDED
#define MENU_HANDLER_H_INCLUDED

#include "WeatherDataStore.h"
#include "Calculator.h"
#include "OutputHandler.h"
#include <string>

    /**
     * @file MenuHandler.h
     * @brief Drives the main menu loop and coordinates user interaction.
     *
     * Reads and validates input, invokes Calculator for computation,
     * and delegates all formatted output to OutputHandler.
     *
     * WeatherDataStore replaces the flat Vector<WeatherRecord> + YearTracker
     * used in Assignment 1, enabling O(log n) data access via the internal map.
     *
     * DisplayAvailableYears calls store.TraverseYears(PrintYear), passing the
     * static function pointer PrintYear into the BST in-order traversal.
     * PrintYear's signature matches Bst<int>::f1Typ exactly: void (*)(int&).
     *
     * @author Deston
     * @version 2.1
     * @date 24/03/2026
     */
class MenuHandler
{
public:
    /** @brief Default constructor. */
    MenuHandler();

    /**
     * @brief Display the main menu and return the user's choice (1-5).
     * @return Integer choice entered by the user.
     */
    int DisplayMenu() const;

    /**
     * @brief Display a welcome message with the total record count.
     * @param recordCount Number of records loaded from all files.
     */
    void DisplayWelcome(int recordCount) const;

    /**
     * @brief Print available years using BST in-order traversal.
     *
     * Calls store.TraverseYears(PrintYear).  PrintYear is a static member
     * whose address is a plain function pointer matching Bst<int>::f1Typ.
     * The BST delivers years in ascending order.
     *
     * @param store  Populated weather data store.
     */
    void DisplayAvailableYears(const WeatherDataStore& store) const;

    /**
     * @brief Option 1 — wind speed mean and stdev for a user-specified month/year.
     * @param store   Indexed weather data store.
     * @param calc    Calculator instance.
     * @param output  OutputHandler instance.
     */
    void HandleOption1(const WeatherDataStore& store,
                       const Calculator& calc,
                       const OutputHandler& output) const;

    /**
     * @brief Option 2 — temperature statistics for all months of a year.
     * @param store   Indexed weather data store.
     * @param calc    Calculator instance.
     * @param output  OutputHandler instance.
     */
    void HandleOption2(const WeatherDataStore& store,
                       const Calculator& calc,
                       const OutputHandler& output) const;

    /**
     * @brief Option 3 — Sample Pearson Correlation Coefficients (S_T, S_R, T_R)
     *        for a user-specified month across ALL loaded years.
     * @param store   Indexed weather data store.
     * @param calc    Calculator instance.
     * @param output  OutputHandler instance.
     */
    void HandleOption3(const WeatherDataStore& store,
                       const Calculator& calc,
                       const OutputHandler& output) const;

    /**
     * @brief Option 4 — write Wind/Temp/Solar CSV with stdev and MAD for a year.
     * @param store   Indexed weather data store.
     * @param calc    Calculator instance.
     * @param output  OutputHandler instance.
     */
    void HandleOption4(const WeatherDataStore& store,
                       const Calculator& calc,
                       const OutputHandler& output) const;

    /** @brief Display the exit message. */
    void DisplayGoodbye() const;

private:
    /**
     * @brief Read and validate an integer from standard input.
     * @param prompt  Text to display before reading.
     * @return Valid integer entered by the user.
     */
    int GetIntInput(const std::string& prompt) const;

    /**
     * @brief Prompt for and validate a month number (1-12).
     * @return Valid month number.
     */
    int GetMonthInput() const;

    /**
     * @brief Prompt for and validate a year that exists in the store.
     * @param store  Used to verify the entered year exists via YearExists().
     * @return Valid year present in the loaded data.
     */
    int GetYearInput(const WeatherDataStore& store) const;

    /** @brief Discard remaining characters in the input buffer. */
    void ClearInput() const;

    /**
     * @brief Static visitor passed to Bst<int>::inOrderTraversal as f1Typ.
     *
     * Signature is void (*)(int&) to match Bst<T>::f1Typ exactly.
     * Must be static so its address is a plain function pointer (not a
     * member pointer), which is what Bst<T>::inOrderTraversal requires.
     * Prints the year value followed by a space.
     *
     * @param year  Year value delivered by the BST traversal.
     */
    static void PrintYear(int& year);
};

#endif // MENU_HANDLER_H_INCLUDED
