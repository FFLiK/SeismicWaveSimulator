#include <SDL.h>
#include <iostream>
#include <Window.h>
#include <Scene.h>
#include <MainScene.h>
#include <FileIO.h>
#include <Log.h>
using namespace std;

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);

	cout << "[Seismic Wave Simulator]" << endl << endl;
	cout << "Github : https://github.com/CLiF-1593/SeismicWaveSimulator" << endl << endl << endl;
	cout << "<Screen Size Setting>" << endl;
	
	ERR_WIDTH:
	cout << "Width (pixel) : " << endl;
	int width;
	cin >> width;
	if (cin.fail() || width <= 0) {
		cin.clear();
		cin.ignore(100000, '\n');
		cout << "> The number of Frames is out of the range." << endl;
		goto ERR_WIDTH;
	}
	cout << endl;

	ERR_HEIGHT:
	cout << "Height (pixel) : " << endl;
	int height;
	cin >> height;
	if (cin.fail() || height <= 0) {
		cin.clear();
		cin.ignore(100000, '\n');
		cout << "> The number of Frames is out of the range." << endl;
		goto ERR_HEIGHT;
	}
	cout << endl;

	string path;
	#if !INF_LAYER
	path = FileIO::OpenFile(CONFIG);
	#endif
	Window* win = new Window({"Seismic Wave Simulator", width, height, 60});
	win->Rendering();

	Scene* scene;
	scene = new MainScene(path);
	win->AddScene(scene, 0);

	bool run = true;
	while (run) {
		switch (win->PollEvent()) {
		case Event::QUIT:
			run = false;
			break;
		}
	}

	delete win;
	SDL_Quit();
	return 0;
}