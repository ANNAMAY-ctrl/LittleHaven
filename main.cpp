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

bool collidesWithCottage(
	sf::Vector2f position,
	sf::Vector2f size,
	sf::Vector2f cottagePosition)
{
	sf::FloatRect playerBounds(position, size);

	sf::FloatRect cottageBounds(
		{
			cottagePosition.x + 15.f,
			cottagePosition.y + 85.f
		},
		{ 130.f, 65.f }
	);

	return playerBounds.findIntersection(cottageBounds).has_value();
}

struct Petal
{
	sf::Vector2f position{ 0.f, 0.f };
	float speed = 0.f;
	float drift = 0.f;
	float rotation = 0.f;
	float size = 0.f;
};

struct Butterfly
{
	sf::Vector2f position{ 0.f, 0.f };
	float speed = 0.f;
	float direction = 0.f;
	float wingTime = 0.f;
	sf::Color color = sf::Color::White;
};

int main()
{
	sf::RenderWindow window(
		sf::VideoMode({ 1280, 720 }), "Little Haven"
	);

	window.setFramerateLimit(60);
	window.setKeyRepeatEnabled(false);

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

		reedsTexture.setSmooth(false);
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

	//Load cottage texture
		sf::Texture cottageTexture;
		if (!cottageTexture.loadFromFile(
			"assets/buildings/cottage.png"))
		{
			return 1;
		}

		cottageTexture.setSmooth(false);

		sf::Sprite cottage(cottageTexture);

		cottage.setScale({
			160.f / cottageTexture.getSize().x,
			160.f / cottageTexture.getSize().y
			});

		cottage.setPosition({ 640.f, 120.f });


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

		static std::mt19937 petalRandom(123);

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
		float waterTime = 0.f;

		//Create butterflies 
		std::vector<Butterfly> butterflies;

		for (int i = 0; i < 8; i++)
		{
			Butterfly butterfly;

			butterfly.position = {
				petalX(petalRandom),
				petalY(petalRandom)
			};

			butterfly.speed = 20.f + i * 3.f;
			butterfly.direction = i * 0.8f;
			butterfly.wingTime = i * 0.5f;


			if (i % 3 == 0)
			{
				butterfly.color = sf::Color(224, 145, 185);
			}
				
			else if (i % 3 == 1)
			{
				butterfly.color = sf::Color(180, 145, 220);
			}
			else
		{
			butterfly.color = sf::Color(245, 210, 125);
		}

			butterflies.push_back(butterfly);
		}
		float butterflyTime = 0.f;

	//Create the farm map
		static int farmMap[MAP_HEIGHT][MAP_WIDTH] = {};
		static int grassVariants[MAP_HEIGHT][MAP_WIDTH] = {};

	static std::mt19937 randomGenerator(42);

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

	//Cottage interactions
	bool insideCottage = false;

	sf::Vector2f outsidePosition{ 0.f, 0.f };

	sf::Vector2f cottageDoor = {
		cottage.getPosition().x + 80.f,
		cottage.getPosition().y + 150.f
	};

	sf::Vector2f interiorDoor = {
		640.f, 640.f
	};


	sf::Clock clock;

	while (window.isOpen())
	{
		float deltaTime = clock.restart().asSeconds();

		//Update falling petals
		windTime += deltaTime;
		waterTime += deltaTime;

		//Update butterfly movement
		butterflyTime += deltaTime;

		for (auto& butterfly : butterflies)
		{
			butterfly.wingTime += deltaTime * 12.f;
			butterfly.direction +=
				std::sin(butterflyTime +
					butterfly.wingTime * 0.05f)
				* deltaTime;

			butterfly.position.x +=
				std::cos(butterfly.direction)
				* butterfly.speed * deltaTime;


			butterfly.position.y +=
				std::sin(butterfly.direction)
				* butterfly.speed * deltaTime;

			//Keep butterflies inside world
			if (butterfly.position.x > 1290.f)
				butterfly.position.x = -10;

			if (butterfly.position.x < -10.f)
				butterfly.position.x = 1290;

			if (butterfly.position.y > 730.f)
				butterfly.position.y = -10.f;

			if (butterfly.position.y < -10.f)
				butterfly.position.y = 730.f;
		}



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

			if (const auto* keyPressed =
				event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->code ==
					sf::Keyboard::Key::E)
				{
					sf::Vector2f feet = {
						player.getPosition().x +
						player.getSize().x / 2.f,

						  player.getPosition().y +
					player.getSize().y
					};

					if (!insideCottage)
					{
						float distance = std::hypot(
							feet.x - cottageDoor.x,
							feet.y - cottageDoor.y
						);

						if (distance < 60.f)
						{
							outsidePosition =
								player.getPosition();

							insideCottage = true;

							player.setPosition({
								620.f, 540.f
								});
						}
					}
					else
					{
						float distance = std::hypot(
							feet.x - interiorDoor.x,
							feet.y - interiorDoor.y
						);

						if (distance < 70.f)
						{
							insideCottage = false;

							player.setPosition(
								outsidePosition);
						}
					}
				}
			}
		}


	
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


		//Cottage interior 
		if (insideCottage)
		{
			sf::Vector2f nextPosition =
				player.getPosition() + movement;

			if (nextPosition.x >= 200.f &&
				nextPosition.x + player.getSize().x <= 1080.f)
			{
				player.move({ movement.x, 0.f });
			}

			if (nextPosition.y >= 160.f &&
				nextPosition.y + player.getSize().y <= 660.f)
			{
				player.move({ 0.f, movement.y });
			}
			window.clear(sf::Color(65, 45, 50));

			//Wooden flooring
			sf::RectangleShape floor({ 880.f, 500.f });
			floor.setPosition({ 200.f, 160.f });
			floor.setFillColor(sf::Color(198, 156, 114));

			window.draw(floor);

			//Wooden floor planks
			for (int row = 0; row < 20; row++)
			{
				for (int col = 0; col < 11; col++)
				{
					sf::RectangleShape plank({ 78.f, 23.f });

					plank.setPosition({
						201.f + col * 80.f,
						161.f + row * 25.f
						});

					int shade = (row * 7 + col * 3) % 15;

					plank.setFillColor(sf::Color(
						210 - shade,
						170 - shade,
						130 - shade
					));

					window.draw(plank);
				}
			}

			//Pink wallpaper
			sf::RectangleShape wall({ 880.f, 100.f });
			wall.setPosition({ 200.f, 60.f });
			wall.setFillColor(sf::Color(235, 190, 204));

			window.draw(wall);

			//Wallpaper stripes
			for (int i = 0; i < 22; i++)
			{
				sf::RectangleShape stripe({ 20.f, 100.f });

				stripe.setPosition({
						200.f + i * 40.f,
						60.f
					});

				stripe.setFillColor(sf::Color(244, 211, 220));

				window.draw(stripe);
			}

			//Floral wallpaper pattern
			for (int row = 0; row < 2; row++)
			{
				for (int col = 0; col < 22; col++)
				{
					float x = 210.f + col * 40.f;
					float y = 85.f + row * 40.f;

					for (int petal = 0; petal < 5; petal++)
					{
						float angle = petal * 6.283185f / 5.f;

						sf::CircleShape flowerPetal(3.f, 8);

						flowerPetal.setPosition({
							x + std::cos(angle) * 4.f - 3.f,
							y + std::sin(angle) * 4.f - 3.f,
							});

						flowerPetal.setFillColor(
							sf::Color(203, 133, 161));

						window.draw(flowerPetal);
					}

					sf::CircleShape center(2.f);
					center.setPosition({ x - 2.f, y - 2.f });
					center.setFillColor(sf::Color(255, 231, 174));

					window.draw(center);
				}
			}

			//Cream skirting board 
			sf::RectangleShape skirting({ 880.f, 12.f });
			skirting.setPosition({ 200.f, 148.f });
			skirting.setFillColor(sf::Color(250, 234, 213));

			window.draw(skirting);

			//Wooden ceiling border
			sf::RectangleShape ceiling({ 880.f, 10.f });
			ceiling.setPosition({ 200.f, 60.f });
			ceiling.setFillColor(sf::Color(153, 111, 82));

			window.draw(ceiling);

			//Exit door
			sf::RectangleShape exitDoor({ 80.f, 20.f });
			exitDoor.setPosition({ 600.f, 640.f });
			exitDoor.setFillColor(sf::Color(125, 85, 55));

			window.draw(exitDoor);

			window.draw(player);
			window.display();

			continue;
		}


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

		if (collidesWithCottage(
			nextY,
			player.getSize(),
			cottage.getPosition()
		))
		{
			blockedY = true;
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

		if (collidesWithCottage(
			nextX,
			player.getSize(),
			cottage.getPosition()
		))
		{
			blockedX = true;
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

					{
						float wave = std::sin(
							waterTime * 1.5f +
							x * 0.45f +
							y * 0.35f);

						int brightness =
							static_cast<int>(240.f + wave * 15.f);

						tile.setFillColor(sf::Color(
							brightness,
							brightness,
							255));
					}
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

		
		// Draw animated water reflections
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			for (int x = 0; x < MAP_WIDTH; x++)
			{
				if (farmMap[y][x] == 2)
				{
					float phase = waterTime * 1.5f +
						x * 2.f + y * 3.f;

					float shimmer = std::sin(phase);

					sf::RectangleShape reflection(
						{ 12.f, 2.f });

					reflection.setPosition({
						x * TILE_SIZE + 14.f +
							shimmer * 8.f,
						y * TILE_SIZE + 20.f
						});

					reflection.setFillColor(
						sf::Color(220, 245, 255,
							static_cast<int>(
								35.f + shimmer * 25.f)));

					window.draw(reflection);
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

		// Calculate the player's ground position
		float playerFeet = player.getPosition().y +
			player.getSize().y;

		// Calculate cottage depth
		float cottageDepth =
			cottage.getPosition().y + 150.f;

		// Draw trees behind the player
		for (const auto& position : treePositions)
		{
			if (position.y + 150.f <= playerFeet)
			{
				tree.setPosition(position);
				window.draw(tree);
			}
		}

		// Draw cottage behind player
		if (cottageDepth <= playerFeet)
		{
			window.draw(cottage);
		}

		// Draw player
		window.draw(player);

		// Draw cottage in front of player
		if (cottageDepth > playerFeet)
		{
			window.draw(cottage);
		}

		// Draw trees in front of the player
		for (const auto& position : treePositions)
		{
			if (position.y + 150.f > playerFeet)
			{
				tree.setPosition(position);
				window.draw(tree);
			}
		}
		
		//Draw animated butterlies 
		for (const auto& butterfly : butterflies)
		{
			float flap = 2.f +
				std::abs(std::sin(butterfly.wingTime)) * 6.f;

			//Left wing
			sf::RectangleShape leftWing(
				{ flap, 8.f });

			leftWing.setFillColor(butterfly.color);

			leftWing.setPosition({
					butterfly.position.x - flap,
					butterfly.position.y - 4.f
				});

			//Right wing
			sf::RectangleShape rightWing(
				{ flap, 8.f });

		rightWing.setFillColor(butterfly.color);

			rightWing.setPosition({
				butterfly.position.x + 2.f,
				butterfly.position.y - 4.f
				});

			//Butterly body
			sf::RectangleShape body(
				{ 2.f, 10.f });

			body.setFillColor(
				sf::Color(75, 55, 70));

			body.setPosition({
				butterfly.position.x,
				butterfly.position.y - 5.f
				});

			window.draw(leftWing);
			window.draw(rightWing);
			window.draw(body);
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