import bpy
from mathutils import Matrix

particle_system = bpy.context.object.particle_systems[0]
for particle in particle_system.particles:
    print(particle.location)