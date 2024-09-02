import pygame
import math

pygame.init()

WIDTH, HEIGHT = 800, 800
WIN = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Ball Simluation")

WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (188, 39, 50)

class Circle:

    g = 9.81

    def __init__(self, x, y, radius, colour, mass):
        self.x = x
        self.y = y
        self.radius = radius
        self.colour = colour
        self.mass = mass

        self.x_velocity = 0
        self.y_velocity = 0

    def draw(self, win):
        pygame.draw.circle(win, self.colour, (self.x, self.y), self.radius)

    def update_position(self):
        self.y_velocity += self.g
        self.y += self.y_velocity / 60

ball = Circle(400, 400, 15, BLACK, 500)
print(ball.bottom)

def main():
    run = True
    clock = pygame.time.Clock()

    while run:
        clock.tick(60)
        WIN.fill(WHITE)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False

        ball.update_position()
        ball.draw(WIN)

        pygame.display.update()
    
    pygame.quit()

main()