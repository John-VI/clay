project('clay', 'c')

sdl3_dep = dependency('sdl3')
sdl3_image_dep = dependency('sdl3_image')

executable('clay', ['src/main.c', 'src/bsd.c', 'src/anidata.c', 'src/text.c'],
		   win_subsystem: 'windows',
		   dependencies: [sdl3_dep, sdl3_image_dep],
		   install: false)