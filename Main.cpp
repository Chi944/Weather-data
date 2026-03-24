#include "Vector.h"
#include "WeatherRecord.h"
#include "DataLoader.h"
#include "DataFileManager.h"
#include "WeatherDataStore.h"
#include "Calculator.h"
#include "OutputHandler.h"
#include "MenuHandler.h"
#include "Constants.h"

#include <iostream>
#include <string>

    /**
     * @file Main.cpp
     * @brief Entry point for the Weather Data Processor (Assignment 2).
     *
     * 1. Reads CSV filenames from data_source.txt via DataFileManager.
     * 2. Loads raw WeatherRecord objects via DataLoader / DataFileManager.
     * 3. Builds a WeatherDataStore (std::map + Bst<int>) from raw records.
     * 4. Runs the interactive menu loop until the user selects Exit (5).
     *
     * @author Deston
     * @date 24/03/2026
     * @return 0 on success, 1 on unrecoverable error.
     */
int main()
{
    // ------------------------------------------------------------------
    // 1. Read filenames from data_source.txt
    // ------------------------------------------------------------------
    DataFileManager fileManager;
    Vector<std::string> filenames;

    int fileCount = fileManager.ReadDataSourceFile(DATA_SOURCE_FILE, filenames);
    if (fileCount == 0)
    {
        std::cerr << "ERROR: No valid filenames found in '"
                  << DATA_SOURCE_FILE << "'" << std::endl;
        std::cerr << "Please ensure the file exists and contains at least one "
                     "CSV filename." << std::endl;
        return 1;
    }

    // ------------------------------------------------------------------
    // 2. Load raw records using existing DataLoader / DataFileManager
    // ------------------------------------------------------------------
    Vector<WeatherRecord> records;
    DataLoader loader;

    std::cout << "Loading data files..." << std::endl;
    std::cout << "=====================" << std::endl;

    int totalRecords =
        fileManager.LoadDataFiles(filenames, records, loader, DATA_FOLDER);

    for (int i = 0; i < fileManager.GetFileCount(); i++)
    {
        std::cout << "Loading: " << filenames[i]
                  << " ... " << fileManager.GetFileResult(i) << std::endl;
    }

    std::cout << "=====================" << std::endl;
    std::cout << "Files loaded: " << fileManager.GetFilesLoaded()
              << "/" << fileManager.GetFileCount() << std::endl;
    std::cout << "Total unique records: " << totalRecords;

    if (fileManager.GetDuplicateCount() > 0)
    {
        std::cout << " (" << fileManager.GetDuplicateCount()
                  << " duplicates filtered)";
    }
    std::cout << std::endl << std::endl;

    if (totalRecords == 0)
    {
        std::cerr << "ERROR: No valid records could be loaded from any file."
                  << std::endl;
        return 1;
    }

    // ------------------------------------------------------------------
    // 3. Build WeatherDataStore (std::map + Bst<int>) from raw records
    // ------------------------------------------------------------------
    WeatherDataStore store;
    store.Build(records);

    if (store.IsEmpty())
    {
        std::cerr << "ERROR: No valid years found in the data." << std::endl;
        return 1;
    }

    // ------------------------------------------------------------------
    // 4. Initialise helpers and run the menu loop
    // ------------------------------------------------------------------
    Calculator    calculator;
    OutputHandler outputHandler;
    MenuHandler   menuHandler;

    menuHandler.DisplayWelcome(records.Size());
    menuHandler.DisplayAvailableYears(store);

    bool running = true;
    while (running)
    {
        int choice = menuHandler.DisplayMenu();

        switch (choice)
        {
        case 1:
            menuHandler.HandleOption1(store, calculator, outputHandler);
            break;
        case 2:
            menuHandler.HandleOption2(store, calculator, outputHandler);
            break;
        case 3:
            menuHandler.HandleOption3(store, calculator, outputHandler);
            break;
        case 4:
            menuHandler.HandleOption4(store, calculator, outputHandler);
            break;
        case 5:
            menuHandler.DisplayGoodbye();
            running = false;
            break;
        default:
            std::cout << std::endl
                      << "Invalid choice. Please enter 1-5." << std::endl;
            break;
        }
    }

    return 0;
}
