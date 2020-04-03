import bpy
import numpy

from mathutils import Matrix

particleFile=open('D:\\projects\\IcedTech\\dkbase\\fx\\particles\\export.prt','w')
particleFile.write("ParticleVersion 3\n")

s=bpy.context.scene

dg = bpy.context.evaluated_depsgraph_get()
ob = bpy.context.object.evaluated_get(dg)
# assume context object has a ps, use active
particle_system = ob.particle_systems.active

particleFile.write(str("NumFrames ") + str(int(s.frame_end-s.frame_start)))
particleFile.write('\n\n')
for i in range(s.frame_start,s.frame_end):
    bpy.context.scene.frame_set(i)
    bpy.context.scene.frame_set(i)
    particleFile.write('ParticleFrame\n')
    particleFile.write('{\n')
    particleFile.write(str('\tNumParticles ') + str(len(particle_system.particles)) + str('\n'))
    for particle in particle_system.particles:
        frametime = (numpy.clip(particle.die_time, s.frame_start,s.frame_end) - i)
        if(particle.alive_state != 'ALIVE') :
            frametime = -1
        particleFile.write('\t')
        particleFile.write(str(float(particle.location.x)) + str(' ') + str(float(particle.location.y)) + str(' ') + str(float(particle.location.z))  + str(' ') + str(particle.alive_state) + str(' ') + str(frametime) + str(' ') + str(particle.size)  )
        particleFile.write('\n')
    particleFile.write('}\n')

particleFile.close();