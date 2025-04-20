#include "KDTree.h"
#include "GridBucketing.h"
#include "DataLoader.h"
#include <pdcurses/curses.h>
#include <fstream>
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <numeric>
#include <cmath>
#include <iomanip>

/**
 * @brief Main entry point for the collision data analysis system.
 *
 * Offers a CLI for loading data, querying, and performance reporting using PDCurses.
 * Background: Enables exploration of energy conversion inefficiencies.
 */
int main() {
    // Initialize PDCurses
    initscr();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);  // Menu color
    init_pair(2, COLOR_YELLOW, COLOR_BLACK); // Prompt color
    init_pair(3, COLOR_RED, COLOR_BLACK);    // Error color
    cbreak();
    noecho();
    keypad(stdscr, true);

    // Create a centered window for the menu
    int height = 15;
    int width = 60;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;
    WINDOW* menu_win = newwin(height, width, starty, startx);
    box(menu_win, 0, 0);
    wbkgd(menu_win, COLOR_PAIR(1));
    refresh();

    // Main menu
    auto printMainMenu = [&]() {
        werase(menu_win);
        box(menu_win, 0, 0);
        mvwprintw(menu_win, 1, 2, "Collision Data Analysis System");
        mvwprintw(menu_win, 2, 2, "-----------------------------");
        mvwprintw(menu_win, 3, 2, "1. Load Data");
        mvwprintw(menu_win, 4, 2, "2. Query Events");
        mvwprintw(menu_win, 5, 2, "3. Generate Performance Report");
        mvwprintw(menu_win, 6, 2, "4. Exit");
        mvwprintw(menu_win, 8, 2, "Press a number to select an option.");
        wrefresh(menu_win);
    };
    printMainMenu();

    std::unique_ptr<DataStructure> ds;
    std::vector<CollisionEvent> events;
    int choice;

	while (true) {
        choice = getch();
        if (choice == '1') {
            // Data structures menu
            werase(menu_win);
            box(menu_win, 0, 0);
            mvwprintw(menu_win, 1, 2, "Choose Data Structure:");
            mvwprintw(menu_win, 2, 2, "1. KDTree");
            mvwprintw(menu_win, 3, 2, "2. GridBucketing");
            wattron(menu_win, COLOR_PAIR(2));
            mvwprintw(menu_win, 5, 2, "Enter choice (1 or 2): ");
            wattroff(menu_win, COLOR_PAIR(2));
            wrefresh(menu_win);
            int dsChoice = getch();

            // Load data into structure
            if (dsChoice == '1') {
                ds = std::make_unique<KDTree>();
            } else if (dsChoice == '2') {
                ds = std::make_unique<GridBucketing>(0, 210);
            } else {
                wattron(menu_win, COLOR_PAIR(3));
                mvwprintw(menu_win, 7, 2, "Invalid choice. Press any key.");
                wattroff(menu_win, COLOR_PAIR(3));
                wrefresh(menu_win);
                getch();
                printMainMenu();
                continue;
            }
            events = loadData("../data/collision_data.bin");

            // Export all events to CSV for visualization
            std::ofstream allEventsOut("../data/all_events.csv");
            allEventsOut << "eventId,incomingParticles,outgoingParticles,kineticEnergyIn,restEnergyOut,efficiency\n";
            for (const auto& e : events) {
                allEventsOut << e.eventId << ","
                             << "\"" << e.incomingParticles.c_str() << "\","
                             << "\"" << e.outgoingParticles.c_str() << "\","
                             << std::fixed << std::setprecision(6) << e.kineticEnergyIn << ","
                             << std::fixed << std::setprecision(6) << e.restEnergyOut << ","
                             << std::fixed << std::setprecision(6) << e.efficiency << "\n";
            }
            allEventsOut.close();
            auto start = std::chrono::high_resolution_clock::now();
            if (dsChoice == '1') {
                dynamic_cast<KDTree*>(ds.get())->buildBalanced(events);
            } else {
                for (const auto& event : events) {
                    ds->insert(event);
                }
            }
            auto end = std::chrono::high_resolution_clock::now();
            long loadTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            mvwprintw(menu_win, 7, 2, "Loaded %d events in %ld ms.", events.size(), loadTime);
            mvwprintw(menu_win, 8, 2, "Exported to all_events.csv.");
            mvwprintw(menu_win, 9, 2, "Press any key to continue.");
            wrefresh(menu_win);
            getch();
            printMainMenu();
        } else if (choice == '2') {
            if (!ds) {
                wattron(menu_win, COLOR_PAIR(3));
                mvwprintw(menu_win, 10, 2, "Load data first! Press any key.");
                wattroff(menu_win, COLOR_PAIR(3));
                wrefresh(menu_win);
                getch();
                printMainMenu();
                continue;
            }
            // Query menu
            werase(menu_win);
            box(menu_win, 0, 0);
            mvwprintw(menu_win, 1, 2, "Query Events:");
            mvwprintw(menu_win, 2, 2, "1. Range Query");
            mvwprintw(menu_win, 3, 2, "2. Extremum Query");
            wattron(menu_win, COLOR_PAIR(2));
            mvwprintw(menu_win, 5, 2, "Enter choice (1 or 2): ");
            wattroff(menu_win, COLOR_PAIR(2));
            wrefresh(menu_win);
            int subChoice = getch();
            if (subChoice == '1') {
                float minRest, maxRest;
                wattron(menu_win, COLOR_PAIR(2));
                mvwprintw(menu_win, 7, 2, "Enter min rest energy (GeV): ");
                wrefresh(menu_win);
                echo();
                wscanw(menu_win, "%f", &minRest);
                mvwprintw(menu_win, 8, 2, "Enter max rest energy (GeV): ");
                wrefresh(menu_win);
                wscanw(menu_win, "%f", &maxRest);
                noecho();
                wattroff(menu_win, COLOR_PAIR(2));
                auto start = std::chrono::high_resolution_clock::now();
                auto results = ds->range_query(minRest, maxRest);
                auto end = std::chrono::high_resolution_clock::now();
                mvwprintw(menu_win, 10, 2, "Found %d events in %ld us",
                          results.size(), std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
                mvwprintw(menu_win, 11, 2, "Results saved to range_query_results.csv");
                mvwprintw(menu_win, 12, 2, "Press any key to continue.");
                wrefresh(menu_win);
                std::ofstream out("../data/range_query_results.csv");
                out << "eventId,incomingParticles,outgoingParticles,kineticEnergyIn,restEnergyOut,efficiency\n";
                for (const auto& event : results) {
                    out << event.eventId << ","
                        << "\"" << event.incomingParticles.c_str() << "\","
                        << "\"" << event.outgoingParticles.c_str() << "\","
                    	<< std::fixed << std::setprecision(4) << event.kineticEnergyIn << ","
                    	<< std::fixed << std::setprecision(4) << event.restEnergyOut << ","
                    	<< std::fixed << std::setprecision(6) << event.efficiency << "," << "\n";
                }
                out.close();
                getch();
                printMainMenu();
            } else if (subChoice == '2') {
                auto start = std::chrono::high_resolution_clock::now();
                CollisionEvent maxEffEvent = ds->find_max_efficiency();
                auto end = std::chrono::high_resolution_clock::now();
				mvwprintw(menu_win, 7, 2, "Max efficiency: %.4f (Event %d)",
                          maxEffEvent.efficiency, maxEffEvent.eventId);
                mvwprintw(menu_win, 8, 2, "Time: %ld us",
                          std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
                mvwprintw(menu_win, 10, 2, "Press any key to continue.");
                wrefresh(menu_win);
                getch();
                printMainMenu();
            } else {
                wattron(menu_win, COLOR_PAIR(3));
                mvwprintw(menu_win, 7, 2, "Invalid choice. Press any key.");
                wattroff(menu_win, COLOR_PAIR(3));
                wrefresh(menu_win);
                getch();
                printMainMenu();
            }
        } else if (choice == '3') {
            if (!ds) {
                wattron(menu_win, COLOR_PAIR(3));
                mvwprintw(menu_win, 10, 2, "Load data first! Press any key.");
                wattroff(menu_win, COLOR_PAIR(3));
                wrefresh(menu_win);
                getch();
                printMainMenu();
                continue;
            }
            werase(menu_win);
            box(menu_win, 0, 0);
            mvwprintw(menu_win, 1, 2, "Generating performance report...");
            wrefresh(menu_win);
            std::ofstream out("../data/performance_results.csv");
            out << "DataStructure,AvgInsertionTime(ms),StdDevInsertionTime(ms),AvgRangeQueryTime(us),StdDevRangeQueryTime(us),AvgExtremumQueryTime(us),StdDevExtremumQueryTime(us),Memory(bytes)\n";
            for (int i = 1; i <= 2; ++i) {
                std::vector<long> insertTimes, rangeTimes, extremumTimes;
                const int numRuns = 100;
                for (int run = 0; run < numRuns; ++run) {
                    if (i == 1) {
                        ds = std::make_unique<KDTree>();
                    } else {
                        ds = std::make_unique<GridBucketing>(0, 210);
                    }

                    // Insertions
                    auto start = std::chrono::high_resolution_clock::now();
                    if (i == 1) {
                        dynamic_cast<KDTree*>(ds.get())->buildBalanced(events);
                    } else {
                        for (const auto& event : events) {
                            ds->insert(event);
                        }
                    }
                    auto end = std::chrono::high_resolution_clock::now();
                    insertTimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

                    // Queries
                    long rangeTime = 0, extremumTime = 0;
                    for (int j = 0; j < 10; ++j) {
                        // Range
                        start = std::chrono::high_resolution_clock::now();
                        ds->range_query(100 + j * 10, 150 + j * 10);
                        end = std::chrono::high_resolution_clock::now();
                        rangeTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

                        // Extremum
                        start = std::chrono::high_resolution_clock::now();
                        ds->find_max_efficiency();
                        end = std::chrono::high_resolution_clock::now();
                        extremumTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                    }
                    rangeTimes.push_back(rangeTime / 10);
                    extremumTimes.push_back(extremumTime / 10);
                }

                // Calculate averages
                double avgInsert = std::accumulate(insertTimes.begin(), insertTimes.end(), 0.0) / numRuns;
                double avgRange = std::accumulate(rangeTimes.begin(), rangeTimes.end(), 0.0) / numRuns;
                double avgExtremum = std::accumulate(extremumTimes.begin(), extremumTimes.end(), 0.0) / numRuns;

                // Calculate standard deviations
                double stdDevInsert = 0, stdDevRange = 0, stdDevExtremum = 0;
                for (int k = 0; k < numRuns; ++k) {
                    stdDevInsert += (insertTimes[k] - avgInsert) * (insertTimes[k] - avgInsert);
                    stdDevRange += (rangeTimes[k] - avgRange) * (rangeTimes[k] - avgRange);
                    stdDevExtremum += (extremumTimes[k] - avgExtremum) * (extremumTimes[k] - avgExtremum);
                }
                stdDevInsert = std::sqrt(stdDevInsert / (numRuns - 1));
                stdDevRange = std::sqrt(stdDevRange / (numRuns - 1));
                stdDevExtremum = std::sqrt(stdDevExtremum / (numRuns - 1));

                // Calculate memory usage
                size_t memory = 100000 * sizeof(CollisionEvent);
                if (i == 1) {
                    memory += 10000 * sizeof(Node);
                } else {
                    memory += 100 * sizeof(Cell);
                }

                // Output to CSV file
                out << (i == 1 ? "KDTree" : "GridBucketing") << ","
                    << std::fixed << std::setprecision(2) << avgInsert << ","
                    << std::fixed << std::setprecision(2) << stdDevInsert << ","
                    << std::fixed << std::setprecision(2) << avgRange << ","
                    << std::fixed << std::setprecision(2) << stdDevRange << ","
                    << std::fixed << std::setprecision(2) << avgExtremum << ","
                    << std::fixed << std::setprecision(2) << stdDevExtremum << ","
                    << memory << "\n";
            }
            out.close();
            mvwprintw(menu_win, 3, 2, "Report saved to performance_results.csv");
            mvwprintw(menu_win, 5, 2, "Press any key to continue.");
            wrefresh(menu_win);
            getch();
            printMainMenu();
        } else if (choice == '4') {
            break;
        } else {
            wattron(menu_win, COLOR_PAIR(3));
            mvwprintw(menu_win, 10, 2, "Invalid option. Press any key.");
            wattroff(menu_win, COLOR_PAIR(3));
            wrefresh(menu_win);
            getch();
            printMainMenu();
        }
	}

    // Clean up PDCurses
    endwin();
    return 0;
}