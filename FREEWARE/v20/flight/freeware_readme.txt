FLIGHT, GAMES, Real-time multi-user flight simulator
!
FLIGHT V3.1 is an OpenVMS (VAX and Alpha AXP) application which provides an
interactive, real-time flight simulation environment. While running FLIGHT, the
user sees a wire-frame representation of the world around them, and a
representation of the aircraft's cockpit. Both are continuously updated in real
time. The keyboard and mouse are used to control the aircraft during the
simulation (e.g., start and stop the engine; deflect the ailerons). 

Some of FLIGHT V3.1's capabilities include:

     o	Extensive Collection of Aircraft and Worlds -- FLIGHT includes 100
	different models of aircraft (both fixed wing and rotary, and some
	vertical take-off and landing craft), along with a few ships, ground
	vehicles, the Apollo/Saturn V rocket, and other odds and ends, some
	real and some fictional (such as the NCC-1701D USS Enterprise).

	More than 20 different worlds are provided in which simulations may be
	run, covering a variety of regions (some actual and some fictional) and
	time eras, as well as some specialized worlds such as the near-Earth
	solar system.

     o	User-Created Aircraft and Worlds -- FLIGHT allows users to create new
	objects (e.g., aircraft and ships) which can then be flown or driven
	in FLIGHT. Similarly, new environments ("worlds") may be created by
	users in which simulations can then take place. Compiler and viewer
	utilities are provided with FLIGHT to facilitate this.

	The definition of an aircraft covers aerodynamic (e.g., lift and
	drag), geometric (appearance and animation), and structural (e.g.,
	wing span and chord, strength) aspects. In addition, the cockpit layout
	is completely user-defined. Instrument panels may be composed of
	arbitrarily arranged user-defined instruments. Instruments themselves
	are defined as of some basic type, ranging from the very general (dial,
	slider, digital, lamp, toggle) to the more specialized (radio, radar,
	artificial horizon, turn-slip/turn coordinator, ADF, VOR/ILS). An
	instrument may be "wired" to more than 70 virtual signal channels such
	as airspeed, altitude, throttle setting, and various radar lock data.
	Customizable aspects of an instrument range from basic parameters such
	as size and textual or numeric labeling, to details such as the shape
	of the toy airplane in an artificial horizon or the curvature and arc
	of the tube in which a turn coordinator's "ball" is suspended.

	The definition of a world covers geometric (appearance and location of
	objects), radio (frequency, type, and location), and physical (e.g.,
	gravitational) aspects.

     o	Multiple Users -- Up to seven users, running separate simulators on
	separate OpenVMS systems, may coexist together in a shared enviroment
	linked by DECnet. Each user will see all other users, and collisions
	between users' aircraft (and weapon fire) may be detected.

	Collisions may result in either partial or complete destruction of one
	or both parties, depending on such factors as the geometry of the
	collision and the strength of the aircraft or weapons. Damage can be
	evidenced in various forms, including partial or total loss of control
	(e.g., a rear hit may affect the aircraft's elevators) or engine power.

	Simulated radios may be used for communication amongst users. Radios
	may be set to different frequencies.

     o	Multi-Crew Aircraft -- An aircraft model may include up to four crew
	positions, such as pilot, copilot, navigator, gunner. Each crew
	position can have unique viewpoints and instruments, and can be
	operated by a separate user on their own workstation or display.

     o	Multiple Windows -- An aircraft may have multiple view windows (one
	facing forward and another aft, for example) and multiple instrument
	panels (e.g., a main console and an overhead panel).

     o	Multiple Viewpoints -- The user may view the environment from various
	points in or around the aircraft being flown (e.g., from the left seat
	or right seat or behind the aircraft), or the user may view the
	aircraft being flown from external viewpoints (control towers,
	mountaintops, etc.).

     o	Animated Aircraft -- An aircraft model may include moving components
	such as landing gear, bomb doors, control surfaces, and the like which
	visually illustrate the changing state of an aircraft, as well as
	autonomous components such as rotating beacons or strobes for night
	flight.

	Another use of animated components in an aircraft is to model a Head
	Up Display (HUD). FLIGHT supports simple HUDs which include velocity
	vectors, aim points, radar lock indicators, altitude/airspeed tapes,
	pitch ladders, and the like.

     o  Comprehensive Aerodynamic and Mechanical Simulation -- including
        modeling of forces on each wing-half separately (which can result in
        behaviors such as fixed-wing autorotations [spins] and adverse yaw),
        ground effect, vectored thrust, and tail dragger vs. tricyce landing
        gear.

     o	IFR Simulation -- FLIGHT models the essential features of both airborne
	and ground-based equipment to support Instrument Flight Rules
	operations. This includes working ADF recievers with NDB ground
	stations, and VOR/ILS receivers with corresponding ground stations.

     o	Multiple Vehicles per Simulation -- A simulated aircraft may deploy
	additional simulated objects. Examples include NASA's B-52N releasing
	an X-15 (provided in the kit); a bomber releasing a cruise missile
	(provided in the kit); a landing craft releasing a tank. The deployed
	object may be controlled separately from the parent craft (with its own
	views, panels, etc.) or it may be self-guiding (e.g., radar-seeking
	missile) or uncontrolled (e.g., a free-falling bomb).

     o	True 3D, Large-Scale Universe -- The environment in which simulations
	occur accurately models not only three-dimensional activity on a local
	scale (e.g., climbs and descents), but on a global and universal scale
	as well. The Earth is a true sphere in FLIGHT. A user can detect this
	via progressive curvature of the horizon as altitude increases, as well
	as by the ability to circumnavigate the globe, returning to one's point
	of origin.

	This latter capability may be achieved in a practical manner with high-
	powered craft such as rockets. FLIGHT's gravity model allows orbital
	behavior to be simulated (in real time), allowing, for example, a
	complete trip around the Earth in about 90 minutes from Low Earth Orbit
	(LEO). The strength of gravity properly attenuates with distance from
	the Earth, and is a function of the mass of the simulated object and
	the Earth. The Apollo/Saturn V model included in the kit has been used
	to achieve stable low Earth orbits lasting a week (real time).

	FLIGHT supports multiple planets (termed "masses") in a single
	environment. Examples include the Earth and the Moon, or two asteroids.
	The masses may have different sizes, masses, and atmospheres.

	The possible extent of a simulated environment is very large. Objects
	and distances may range from the very small (e.g., details of doorways
	on buildings) to the very large (e.g., planets separated by a few
	light-years) within a single environment, all to a consistent linear
	scale.

     o	Terrain Handling -- A simulated enviroment may include arbitrary
	landforms or other impeneterable objects. FLIGHT models the surface
	contours of such objects, to allow for ridges, valleys, tunnels,
	bridges, etc. which may be driven over with surface vehicles or flown
	around/through with air vehicles. Separate land and water surfaces may
	exist on which only suitably equipped vehicles can operate (e.g.,
	automobiles, ships, or amphibious craft).
