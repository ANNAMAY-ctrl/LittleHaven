
#include <SFML/Graphics.hpp>
const int TILE_SIZE = 40;
const int MAP_WIDTH = 32;
const int MAP_HEIGHT = 18;

bool canMove(sf::Vector2f position, 
	sf::Vector2f size,
	const int farmMap[MAP_HEIGHT][MAP_WIDTH])
{
//Check world boundaries
	if (position.x < 0 || position.y < 0 ||
		position.x + size.x > MAP_WIDTH * TILE_SIZE ||
		position.y + size.y > MAP_HEIGHT * TILE_SIZE)
	{
		return false;
	}
	//Find the tiles occupied by the player
	int left = static_cast<int>(position.x / TILE_SIZE);
	int right = static_cast<int>(
		(position.x + size.x - 0.01f) / TILE_SIZE);

	int top = static_cast<int>(position.y / TILE_SIZE);
	int bottom = static_cast<int>(
		(position.y + size.y - 0.01f) / TILE_SIZE);

	//Check for water
	for (int y = top; y <= bottom; y++)
	{
		for (int x = left; x <= right; x++)
		{
			if (farmMap[y][x] == 2)
				return false;
		}
	}
	return true;
}



int main()
{
	sf::RenderWindow window(
		sf::VideoMode({ 1280, 720 }), "Little Haven"
	);

	window.setFramerateLimit(60);
	
	//Create the farm map
	int farmMap[MAP_HEIGHT][MAP_WIDTH] = {};


	//Generate terrain
	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			//Farming area
			if (x >= 3 && x <= 10 && y >= 3 && y <= 9)
				farmMap[y][x] = 1;

				//Pond
			else if (x >= 23 && x <= 28 && y >= 3 && y <= 7)
				farmMap[y][x] = 2;

			//Walking path
			else if (y == 12 || x == 15)
				farmMap[y][x] = 3;

			//Grass
			else
				farmMap[y][x] = 0;

		}
	}

	//Create player
	sf::RectangleShape player({ 40.f, 50.f });
	player.setFillColor(sf::Color(110, 75, 150));
	player.setPosition({ 620.f, 335.f });


	//Movement speed
	float speed = 250.f;

	sf::Clock clock;

	while (window.isOpen())
	{
		float deltaTime = clock.restart().asSeconds();

		while (const auto event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
		}
	

	//Player movement
		// Player movement
		sf::Vector2f movement({ 0.f, 0.f });

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
			movement.y -= speed * deltaTime;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
			movement.y += speed * deltaTime;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
			movement.x -= speed * deltaTime;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
			movement.x += speed * deltaTime;

	//Horizontal collision
	if (canMove(player.getPosition() +
		sf::Vector2f(0.f, movement.y),
		player.getSize(), farmMap))
	{
		player.move({ movement.x, 0.f });
	}

	//Vertical collision
	if (canMove(player.getPosition() +
		sf::Vector2f(movement.y, 0.f),
		player.getSize(), farmMap))
	{
		player.move({ 0.f, movement.y });
	}


	//Draw the game World
	window.clear();

	sf::RectangleShape tile({ 40.f, 40.f });

	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			tile.setPosition({
				x * 40.f,
				y * 40.f
				});

			switch (farmMap[y][x])
			{
			case 0:
				tile.setFillColor(sf::Color(145, 190, 120));
				break;

			case 1:
				tile.setFillColor(sf::Color(150, 99, 63));
				break;

			case 2:
				tile.setFillColor(sf::Color(120, 185, 216));
				break;

			case 3:
				tile.setFillColor(sf::Color(219, 195, 152));
				break;


			}

			window.draw(tile);
		}

	}
	window.draw(player);
	window.display();
}

return 0;
}	