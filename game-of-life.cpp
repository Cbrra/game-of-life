#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>

using namespace std;

const int WIDTH = 1000;
const int HEIGHT = 600;
const int CELL_SIZE = 10;
const int CELL_COUNT_X = WIDTH / CELL_SIZE;
const int CELL_COUNT_Y = HEIGHT / CELL_SIZE;

class GameOfLife
{
private:
    bool paused;
    int generation;
    
    vector<vector<bool>> cells;
    vector<vector<bool>> nextCells;
    vector<vector<sf::RectangleShape>> pixels;

    sf::RenderWindow* window;
    sf::Font* font;

    sf::Text pauseText;
    sf::Text genText;
public:
    GameOfLife(sf::RenderWindow* _window, sf::Font* _font)
        : cells(CELL_COUNT_X, vector<bool>(CELL_COUNT_Y, false)),
        nextCells(CELL_COUNT_X, vector<bool>(CELL_COUNT_Y, false)),
        pixels(CELL_COUNT_X, vector<sf::RectangleShape>(CELL_COUNT_Y))
    {
        paused = true;
        generation = 0;
        window = _window;
        font = _font;

        init();
    }

    void run()
    {
        bool isAdding = false;
        bool isRemoving = false;
        sf::Vector2i lastCell;

        while (window->isOpen())
        {
            sf::Event event;
            while (window->pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window->close();
                }
                else if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Space)
                    {
                        paused = !paused;
                    }
                    else if (event.key.code == sf::Keyboard::R)
                    {
                        reset();
                    }
                }
                else
                {
                    if (event.type == sf::Event::MouseButtonPressed)
                    {
                        if (!isRemoving && event.mouseButton.button == sf::Mouse::Left)
                        {
                            isAdding = true;
                        }
                        else if (!isAdding && event.mouseButton.button == sf::Mouse::Right)
                        {
                            isRemoving = true;
                        }
                    }
                    else if (event.type == sf::Event::MouseButtonReleased)
                    {
                        if (isAdding && event.mouseButton.button == sf::Mouse::Left)
                        {
                            isAdding = false;
                            lastCell.x = -1;
                            lastCell.y = -1;
                        }
                        else if (isRemoving && event.mouseButton.button == sf::Mouse::Right)
                        {
                            isRemoving = false;
                            lastCell.x = -1;
                            lastCell.y = -1;
                        }
                    }

                    if(paused && (isAdding || isRemoving))
                    {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
                        int x = mousePos.x / CELL_SIZE;
                        int y = mousePos.y / CELL_SIZE;

                        if (x >= 0 && x < CELL_COUNT_X && y >= 0 && y < CELL_COUNT_Y && (lastCell.x != x || lastCell.y != y))
                        {
                            lastCell.x = x;
                            lastCell.y = y;
                            generation = 0;

                            cells[x][y] = isAdding; // Toggle the cell state
                            nextCells[x][y] = cells[x][y]; // Update the next grid as well
                        }
                    }
                }
            }

            if (!paused)
            {
                update();
            }

            draw();
        }
    }
private:
    void init()
    {
        // Init pixels
        for (int i = 0; i < CELL_COUNT_X; ++i)
        {
            for (int j = 0; j < CELL_COUNT_Y; ++j)
            {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
                cell.setPosition(i * CELL_SIZE, j * CELL_SIZE);

                pixels[i][j] = cell;
            }
        }

        // Init texts
        pauseText.setFont(*font);
        pauseText.setString("EDIT MODE");
        pauseText.setCharacterSize(30);
        pauseText.setFillColor(sf::Color::White);
        pauseText.setStyle(sf::Text::Bold);
        pauseText.setPosition(WIDTH / 2 - pauseText.getLocalBounds().width / 2, 20);

        genText.setFont(*font);
        genText.setCharacterSize(15);
        genText.setFillColor(sf::Color::White);
        genText.setStyle(sf::Text::Bold);
        genText.setPosition(20, 20);
    }

    void update()
    {
        for (int i = 0; i < CELL_COUNT_X; ++i)
        {
            for (int j = 0; j < CELL_COUNT_Y; ++j)
            {
                int neighbors = getNeighborsCount(i, j);

                if (cells[i][j])
                {
                    if (neighbors < 2 || neighbors > 3)
                        nextCells[i][j] = false;
                    else
                        nextCells[i][j] = true;
                }
                else
                {
                    if (neighbors == 3)
                        nextCells[i][j] = true;
                    else
                        nextCells[i][j] = false;
                }
            }
        }

        swap(cells, nextCells);
        ++generation;
    }

    int getNeighborsCount(int x, int y)
    {
        int count = 0;
        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                if (i == 0 && j == 0)
                    continue;

                int nx = x + i;
                int ny = y + j;

                if (nx >= 0 && nx < CELL_COUNT_X && ny >= 0 && ny < CELL_COUNT_Y)
                {
                    if (cells[nx][ny])
                        count++;
                }
            }
        }
        return count;
    }

    void reset()
    {
        for (int i = 0; i < CELL_COUNT_X; ++i)
        {
            for (int j = 0; j < CELL_COUNT_Y; ++j)
            {
                cells[i][j] = false;
                nextCells[i][j] = false;
            }
        }

        generation = 0;
    }

    void draw()
    {
        window->clear(sf::Color::Black);

        for (int i = 0; i < CELL_COUNT_X; ++i)
        {
            for (int j = 0; j < CELL_COUNT_Y; ++j)
            {
                if (cells[i][j])
                {
                    pixels[i][j].setFillColor(sf::Color::White);
                }
                else
                {
                    pixels[i][j].setFillColor(sf::Color(100, 100, 100)); // Gray color for dead cells
                }

                window->draw(pixels[i][j]);
            }
        }

        if (paused)
        {
            window->draw(pauseText);
        }

        genText.setString("Generation " + to_string(generation));
        window->draw(genText);

        window->display();
    }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Game of Life");
    window.setFramerateLimit(120);

    sf::Font font;
    if (!font.loadFromFile("fonts/Roboto-Regular.ttf"))
    {
        cout << "Unable to load fonts/Roboto-Regular.ttf!" << endl;
        return 0;
    }

    GameOfLife game(&window, &font);
    game.run();

    return 0;
}