
#include <SFML/Graphics.hpp>

int main()
{
	sf::RenderWindow window(
		sf::VideoMode({ 1280, 720 }), "Little Haven"
	);

	window.setFramerateLimit(60);

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
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		player.move({ 0.f,-speed * deltaTime });

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		player.move({ 0.f, speed * deltaTime });

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		player.move({ -speed * deltaTime, 0.f });

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		player.move({ speed * deltaTime, 0.f });


	//Draw the game World
	window.clear(sf::Color(145, 190, 120));
	window.draw(player);
	window.display();
}

return 0;
}	