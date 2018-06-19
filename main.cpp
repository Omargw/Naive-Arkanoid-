#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>

using namespace std;
using namespace sf;

constexpr unsigned int windowWidth{ 800 }, windowHeight{ 600 };
constexpr float ballRadius{ 10.0f }, ballVelocity{ 8.0f };
constexpr float paddleWidth{ 60.0f }, paddleHeight{ 20.0f }, paddleVelocity{ 8.0f };

constexpr float blockWidth{ 60.0f }, blockHeight{ 20.0f };
constexpr int contBlockX{ 11 }, countBlockY{ 6 }; // 11 x 4 bricks to play with  = 44

struct Ball{
	CircleShape shape;
	Vector2f velocity{ -ballRadius, -ballRadius };
	Texture img;
	Ball(float mx, float my){
		shape.setPosition(mx, my);
		shape.setRadius(ballRadius);
		
		shape.setFillColor(Color::Red);
		 
		shape.setOrigin(ballRadius, ballRadius);
	}
	void update(){ 
		shape.move(velocity); 

		if (left() < 0)
			velocity.x = ballVelocity;
		else if (right() > windowWidth)
			velocity.x = -ballVelocity;

		if (top() < 0)
			velocity.y = ballVelocity;
	else if (bottom() > windowHeight)
			velocity.y = -ballVelocity;


	
	}

	float x(){ return shape.getPosition().x; }
	float y(){ return shape.getPosition().y; }
	float left(){ return x() - shape.getRadius(); }
	float right(){ return x() + shape.getRadius(); }
	float top(){ return y() - shape.getRadius(); }
	float bottom(){ return y() + shape.getRadius(); }

};

struct Paddle{
	RectangleShape shape;
	Vector2f velocity; // { paddleVelocity, 0 };
	Paddle(float mx, float my){
		shape.setPosition(mx, my);
		shape.setSize({ paddleWidth, paddleHeight });
		shape.setFillColor(Color::Blue);
		shape.setOrigin(paddleWidth/2.f, paddleHeight/2.f);
	}
	void update(){
		shape.move(velocity);

		if (isLeft() && left() > 0)
			velocity.x = -paddleVelocity;
		else if (isRight() && right() < windowWidth)
			velocity.x = paddleVelocity;
		else
			velocity.x = 0.0f;	
		/*if (Keyboard::isKeyPressed(Keyboard::Key::Left) && left() > 0)
			velocity.x = -paddleVelocity;
		else if (Keyboard::isKeyPressed(Keyboard::Key::Right) &&
			right() < windowWidth)
			velocity.x = paddleVelocity;
		else
			velocity.x = 0;*/

	}

	float x(){ return shape.getPosition().x; }
	float y(){ return shape.getPosition().y; }
	float left(){ return x() - (shape.getSize().x/2.0f); }
	float right(){ return x() + (shape.getSize().x/2.0f); }
	float top(){ return y() - (shape.getSize().y/2.0f); }
	float bottom(){ return y() + (shape.getSize().y/2.0f); }

	bool isLeft(){
		if (Keyboard::isKeyPressed(Keyboard::Key::Left)) return true;
		else return false;
	}
	bool isRight(){
		if (Keyboard::isKeyPressed(Keyboard::Key::Right)) return true;
		else return false;
	}
};

struct Brick{
	RectangleShape shape;
	 
	bool isDestroyd{ false };

	Brick(float mx, float my){
		shape.setPosition(mx, my);
		shape.setSize({ blockWidth, blockHeight });
		shape.setFillColor(Color::Yellow);
		shape.setOrigin(blockWidth / 2.f, blockHeight / 2.f);
	}
	 

	float x(){ return shape.getPosition().x; }
	float y(){ return shape.getPosition().y; }
	float left(){ return x() - (shape.getSize().x / 2.0f); }
	float right(){ return x() + (shape.getSize().x / 2.0f); }
	float top(){ return y() - (shape.getSize().y / 2.0f); }
	float bottom(){ return y() + (shape.getSize().y / 2.0f); }
 
};


template<typename T1, typename T2> bool isIntersecting(T1& mA, T2& mB){

	bool A = (mA.right() >= mB.left()) && (mA.left() <= mB.right());
	bool B = (mA.bottom() >= mB.top()) && (mA.top() <= mB.bottom());
	return (A && B);
}
void testCollision(Paddle& mPaddle, Ball& mBall){

	if (!isIntersecting<Paddle, Ball>(mPaddle, mBall)) return;

	mBall.velocity.y = -ballVelocity;

	if (mPaddle.x() < mBall.x())
		mBall.velocity.x = -ballVelocity;
	else
		mBall.velocity.x = ballVelocity;

}

void testCollision(Brick& mBrick, Ball& mBall){

	if (!isIntersecting(mBrick, mBall))return;

	mBrick.isDestroyd = true;

	float overlapLeft{ mBall.right() - mBrick.left() };
	float overlapRight{ mBrick.right() - mBall.left() };
	float overlapTop{ mBall.bottom() - mBrick.top() };
	float overlapBottom{ mBrick.bottom() - mBall.top() };

	bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));
	bool ballFromTop(abs(overlapTop) < abs(overlapBottom));

	float minOverlapX{ ballFromLeft ? overlapLeft : overlapRight };
	float minOverlapY{ ballFromTop ? overlapTop : overlapBottom };

	if (abs(minOverlapX) < abs(minOverlapY))
		mBall.velocity.x = ballFromLeft ? -ballVelocity : ballVelocity;
	else
		mBall.velocity.y = ballFromTop ? -ballVelocity : ballVelocity;
}

 

int main()
{
	vector<Brick> bricks;
	Ball ball{ windowWidth / 2, windowHeight / 2 };
	Paddle paddle{ windowWidth / 2.0f, windowHeight - 50.f };

	for (int x = 0; x < contBlockX; ++x){
		for (int y = 0; y < countBlockY; ++y){
				bricks.emplace_back((x + 1)*(blockWidth + 3) + 22, (y + 2)*(blockHeight+ 3));
		}
	}

	//Ball ball{0 , 0 };
	RenderWindow window{ VideoMode{ windowWidth, windowHeight }, "Arkanoid 1.0" };
	window.setFramerateLimit(60);
	while (true)
	{
		window.clear(Color::Black);
		if (Keyboard::isKeyPressed(Keyboard::Key::Escape)) break;
		
		/*uncomment to pause the game when hit bottom as a game over */
		//if ((ball.y() >= windowHeight)) continue;

		ball.update();
		paddle.update();
		testCollision(paddle, ball);
		for (auto& brick : bricks) testCollision(brick, ball);
		bricks.erase(remove_if(begin(bricks), end(bricks),
			[](const Brick& mBrick)
		{
			return mBrick.isDestroyd;
		}),
			end(bricks));

		window.draw(ball.shape);
		window.draw(paddle.shape);
		for (auto &b : bricks)
			window.draw(b.shape);
		
		window.display();


	}

	return 0;
}
