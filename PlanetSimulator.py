import pygame
import math

pygame.init()

WIDTH, HEIGHT = 800, 800
WIN = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("planet simulator")

drawing = False

WHITE = (255, 255, 255)
YELLOW = (255, 255, 0)
BLUE = (100, 149, 237)
RED = (188, 39, 50)
DARK_GREY = (80, 78, 81)
SILVER = (192, 192, 192)

class Planet:

    AU = 1.496e11
    G = 6.67430e-11
    SCALE = 225 / AU # AU is 100 pixels
    TIMESTEP = 86400 # 1 day per second

    def __init__(self, x, y, radius, colour, mass):
        self.x = x
        self.y = y
        self.radius = radius
        self.colour = colour
        self.mass = mass

        self.orbit = []
        self.sun = False
        self.distance_to_sun = 0

        self.x_vel = 0
        self.y_vel = 0
    
    def draw(self, win):
        x = self.x * self.SCALE + WIDTH / 2
        y = self.y * self.SCALE + HEIGHT / 2

        if len(self.orbit) > 2:
            updated_points = []
            for point in self.orbit:
                orbit_x, orbit_y = point
                orbit_x = orbit_x * self.SCALE + WIDTH / 2
                orbit_y = orbit_y * self.SCALE + WIDTH / 2
                updated_points.append((orbit_x, orbit_y))
            
            pygame.draw.lines(win, self.colour, False, updated_points, 2)

        pygame.draw.circle(win, self.colour, (x, y), self.radius)

    def attraction(self, other):
        other_x, other_y = other.x, other.y
        distance_x = other_x - self.x
        distance_y = other_y - self.y
        distance = math.sqrt(distance_x**2 + distance_y**2)

        if other.sun:
            self.distance_to_sun = distance
        
        force = (self.G * self.mass * other.mass) / distance**2
        theta = math.atan2(distance_y, distance_x)
        force_x = math.cos(theta) * force
        force_y = math.sin(theta) * force
        return force_x, force_y

    def update_position(self, planets):
        total_fx = total_fy = 0
        for planet in planets:
            if self == planet:
                continue
            fx, fy = self.attraction(planet)
            total_fx += fx
            total_fy += fy
        
        self.x_vel += total_fx / self.mass * self.TIMESTEP
        self.y_vel += total_fy / self.mass * self.TIMESTEP

        self.x += self.x_vel * self.TIMESTEP
        self.y += self.y_vel * self.TIMESTEP
        self.orbit.append((self.x, self.y))

def main():
    run = True
    clock = pygame.time.Clock()

    sun = Planet(0, 0, 30, YELLOW, 1.98892e30)
    sun.sun = True

    mercury = Planet(-0.397*Planet.AU, 0, 8, DARK_GREY, 3.3e23)
    mercury.y_vel = 4.74e4
    venus = Planet(-0.723*Planet.AU, 0, 14, WHITE, 4.8685e24)
    venus.y_vel = 3.502e4
    earth = Planet(-1*Planet.AU, 0, 16, BLUE, 5.9742e24)
    earth.y_vel = 2.9783e4
    mars = Planet(-1.524*Planet.AU, 0, 12, RED, 6.39e23)
    mars.y_vel = 2.3007e4

    planets = [sun, earth, mars, mercury, venus]

    while run:
        clock.tick(60)
        WIN.fill((0, 0, 0))

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False

            elif event.type == pygame.MOUSEBUTTONDOWN:
                if event.button == 1:
                    global drawing
                    drawing = True
                    og_mouse_x, og_mouse_y = event.pos
                    print(og_mouse_x, og_mouse_y)

            elif event.type == pygame.MOUSEBUTTONUP:
                if event.button == 1:
                    drawing = False
                    print("stopped drawing")

            elif event.type == pygame.MOUSEMOTION:
                if drawing:
                    mouse_x, mouse_y = event.pos
                    offset_x = og_mouse_x - mouse_x
                    offset_y = og_mouse_y - mouse_y
                    radius = math.sqrt(offset_x ** 2 + offset_y ** 2)
                    pygame.draw.circle(WIN, SILVER, (og_mouse_x, og_mouse_y), radius)
            
        
        
        for planet in planets:
            planet.update_position(planets)
            planet.draw(WIN)

        pygame.display.update()
    
    pygame.quit()

main()