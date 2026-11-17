#include <vector>
#include <cmath>
#include <random>
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

bool isShoreline(int x, int y,
	const int farmMap[MAP_HEIGHT][MAP_WIDTH])
{
	if (farmMap[y][x] != 0)
		return false;

	if (x > 0 && farmMap[y][x - 1] == 2)
		return true;

	if (x < MAP_WIDTH - 1 && farmMap[y][x + 1] == 2)
		return true;


	if (y > 0 && farmMap[y - 1][x] == 2)
		return true;

	if (y < MAP_HEIGHT - 1 && farmMap[y + 1][x] == 2)
		return true;

	return false;
}

bool collidesWithTree(
	sf::Vector2f position,
	sf::Vector2f size,
	const sf::Vector2f& treePosition)
{

	sf::FloatRect playerBounds(position, size);

	sf::FloatRect trunkBounds(
		{ treePosition.x + 45.f,
		treePosition.y + 125.f },
		{ 30.f, 25.f }
	);

	return playerBounds.findIntersection(
		trunkBounds).has_value();
}

struct Petal
{
	sf::Vector2f position;
	float speed;
	float drift;
	float rotation;
	float size;
};

int main()
{
	sf::RenderWindow window(
		sf::VideoMode({ 1280, 720 }), "Little Haven"
	);

	window.setFramerateLimit(60);

	//Load grass texture
	sf::Texture grassTexture;

	if (!grassTexture.loadFromFile("assets/terrain/grass_tileset.png"))
	{
		return 1;
	}
	grassTexture.setSmooth(false);

	//Load water texture
	sf::Texture waterTexture;

	if (!waterTexture.loadFromFile("assets/terrain/water.png"))
	{
		return 1;
	}
	waterTexture.setSmooth(false);

	//Load lily texture
	sf::Texture lilyTexture;

	if (!lilyTexture.loadFromFile("assets/decorations/lily.png"))
	{
		return 1;
	}

	lilyTexture.setSmooth(false);

	//Create lily sprite
	sf::Sprite lily(lilyTexture);

	//Load mossy rock texture
	sf::Texture rockTexture;

	if (!rockTexture.loadFromFile("assets/decorations/mossy_rock.png"))
	{
		return 1;
	}

	rockTexture.setSmooth(false);
		sf::Sprite rock(rockTexture);


		//Load reeds texture 
		sf::Texture reedsTexture;

		if (!reedsTexture.loadFromFile("assets/decorations/reeds.png"))
		{
			return 1;
		}

		rockTexture.setSmooth(false);
		sf::Sprite reeds(reedsTexture);

		//Load cherry blossom texture
		sf::Texture treeTexture;

		if (!treeTexture.loadFromFile("assets/decorations/cherry_blossom.png"))
		{
			return 1;
		}
		treeTexture.setSmooth(false);

		sf::Sprite tree(treeTexture);

		//Load mushroomTexture
		sf::Texture mushroomTexture;

		if (!mushroomTexture.loadFromFile(
			"assets/decorations/mushrooms.png"))
		{
			return 1;
		}

		mushroomTexture.setSmooth(false);

		sf::Sprite mushroom(mushroomTexture);

		//Mushroom locations
		sf::Vector2f mushroomPositions[] =
		{
			{ 125.f, 85.f },	
			{ 490.f, 175.f },
			{ 735.f, 145.f },
			{ 90.f, 640.f },
			{ 450.f, 620.f },
			{ 875.f, 610.f },
			{ 1100.f, 660.f },
			{ 690.f, 400.f },
			{ 470.f, 350.f }
		};



		//Cherry blossom tree locations
		sf::Vector2f treePositions[] =
		{
			{ 20.f, 10.f },
		    { 440.f, 40.f },
			{ 760.f, 20.f },
			{ 40.f, 530.f },
			{ 360.f, 540.f },
			{ 800.f, 530.f },
			{ 1140.f, 540.f }
		};

		//Cherry blossom petals
		std::vector<Petal> petals;

		std::mt19937 petalRandom(123);

		std::uniform_real_distribution<float>
			petalX(0.f, 1280.f),
			petalY(0.f, 720.f),
			petalSpeed(15.f, 35.f),
			petalDrift(8.f, 22.f),
			petalSize(3.f, 6.f),
			petalRotation(0.f, 360.f);

		for (int i = 0; i < 60; i++)
		{
			Petal petal;

			petal.position = {
				petalX(petalRandom),
				petalY(petalRandom)
			};

			petal.speed = petalSpeed(petalRandom);
			petal.drift = petalDrift(petalRandom);
			petal.rotation = petalRotation(petalRandom);
			petal.size = petalSize(petalRandom);

				petals.push_back(petal);
		}
		float windTime = 0.f;

	//Create the farm map
	int farmMap[MAP_HEIGHT][MAP_WIDTH] = {};
	int grassVariants[MAP_HEIGHT][MAP_WIDTH] = {};

	std::mt19937 randomGenerator(42);

	std::discrete_distribution<int> grassDistribution(
		{ 35, 25, 20, 7, 7, 6 }
	);

	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			grassVariants[y][x] =
				grassDistribution(randomGenerator);
		}
	}


	//Generate terrain
	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			//Farming area
			if (x >= 3 && x <= 10 && y >= 3 && y <= 9)
				farmMap[y][x] = 1;

			//Pond
			else if (

				((x - 25.5f) * (x - 25.5f)) / 12.25f +
				((y - 5.f) * (y - 5.f)) / 9.f <= 1.f
				)
			{
				farmMap[y][x] = 2;
			}


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

		//Update falling petals
		windTime += deltaTime;

		for (auto& petal : petals)
		{
			petal.position.y += petal.speed * deltaTime;

			petal.position.x +=
				(petal.drift +
					std::sin(windTime * 2.f +
						petal.rotation * 0.0174533f)
				* 10.f)* deltaTime;

			petal.rotation += 30.f * deltaTime;

			if (petal.position.y > 730.f)
			{
				petal.position.y = -10.f;
				petal.position.x = petalX(petalRandom);
			}

			if (petal.position.x > 1290.f)
			{
				petal.position.x = -10.f;
			}
		}

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

		//Vertical collision
		sf::Vector2f nextY = player.getPosition() +
			sf::Vector2f(0.f, movement.y);
		bool blockedY = false;

		for (const auto& position : treePositions)
		{
			if (collidesWithTree(
				nextY, player.getSize(), position))
			{
				blockedY = true;
				break;
			}
		}
		
		if (!blockedY && canMove(
			nextY, player.getSize(), farmMap))
		{
			player.move({ 0.f, movement.y });
		}

		//Horizontal collison
		sf::Vector2f nextX = player.getPosition() +
			sf::Vector2f(movement.x, 0.f);

		bool blockedX = false;

		for (const auto& position : treePositions)
		{
			if (collidesWithTree(
				nextX, player.getSize(), position))
			{
				blockedX = true;
				break;
			}
		}

		if (!blockedX && canMove(
			nextX, player.getSize(), farmMap))
		{
			player.move({ movement.x, 0.f });
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

				tile.setTexture(nullptr);
				tile.setFillColor(sf::Color::White);


				switch (farmMap[y][x])
				{
				case 0:
					tile.setTexture(&grassTexture, true);
					tile.setTextureRect(sf::IntRect(
						{ grassVariants[y][x] * TILE_SIZE, 0 },
						{ TILE_SIZE, TILE_SIZE }
					));
					break;

				case 1:
					tile.setFillColor(sf::Color(150, 99, 63));
					break;

				case 2:
					tile.setTexture(&waterTexture, true);

					if ((x + y) % 3 == 0)
						tile.setFillColor(sf::Color(225, 240, 255));
					else
						tile.setFillColor(sf::Color::White);
					break;


				case 3:
					tile.setFillColor(sf::Color(219, 195, 152));
					break;


				}

				window.draw(tile);
			}

		}
		//Draw water lilies
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			for (int x = 0; x < MAP_WIDTH; x++)
			{
				if (farmMap[y][x] == 2 &&
					(x * 7 + y * 3) % 9 == 0)
				{
			lily.setPosition({
				  x * 40.f,
				  y * 40.f
				});
			window.draw(lily);
		}
	}
}

		// Draw shoreline decorations
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			for (int x = 0; x < MAP_WIDTH; x++)
			{
				if (isShoreline(x, y, farmMap))
				{
					if ((x + y) % 3 == 0)
					{
						rock.setPosition({
							x * 40.f,
							y * 40.f
							});

						window.draw(rock);
					}
					else if ((x + y) % 3 == 1)
					{
						reeds.setPosition({
							x * 40.f,
							y * 40.f
							});

						window.draw(reeds);
					}
				}
			}
		}

		//Draw mushrooms
		int mushroomIndex = 0;
		
		for (const auto& position : mushroomPositions)
		{
			float scale = 0.6f +
				(mushroomIndex % 3) * 0.2f;

			mushroom.setScale({ scale, scale });
			mushroom.setPosition(position);

			window.draw(mushroom);

			mushroomIndex++;
		}


		/// Calculate the player's ground position
		float playerFeet = player.getPosition().y +
			player.getSize().y;

		// Draw trees behind the player
		for (const auto& position : treePositions)
		{
			if (position.y + 150.f <= playerFeet)
			{
				tree.setPosition(position);
				window.draw(tree);
			}
		}

		//Draw player
		window.draw(player);

			//Draw trees in front of the player
		for (const auto& position : treePositions)
		{
			if (position.y + 150.f > playerFeet)
			{
				tree.setPosition(position);
				window.draw(tree);
			}
			}
		
		//Draw cherry blossom petals
		for (const auto& petal : petals)
		{
			sf::RectangleShape petalShape(
				{ petal.size, petal.size * 0.65f });

			petalShape.setOrigin(
				{ petal.size / 2.f,
				  petal.size * 0.325f });

			petalShape.setFillColor(
				sf::Color(255, 183, 211, 220));
			
			petalShape.setPosition(petal.position);

			petalShape.setRotation(
				sf::degrees(petal.rotation));
			
			window.draw(petalShape);
		}

	window.display();
}

return 0;
}	