project('clay', 'c', default_options: ['warning_level=2','optimization=3'])

sdl3_dep = dependency('sdl3')
sdl3_image_dep = dependency('sdl3-image')

overrides = []

if get_option('debug')
	add_project_arguments('-ggdb3', language: 'c')
	overrides += 'optimization=g'
endif

# add_project_arguments('-O3', language: 'c')

executable('clay', ['src/main.c', 'src/bsd.c', 'src/anidata.c', 'src/text.c', 'src/cons.c', 'src/aniloader.c', 'src/menu.c'],
		   win_subsystem: 'windows',
		   dependencies: [sdl3_dep, sdl3_image_dep],
		   install: false,
		   override_options: overrides)
