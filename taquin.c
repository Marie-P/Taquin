#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#define gridSize 4
#define blendLevel rand() % (15-5) + 5

// Cette structure nous permettra d'enregistrer les coordonnées d'une case
typedef struct 
{
	int line;
	int col;
} Compartment;

// On va enregistrer la réponse
typedef struct 
{
	int *pointer;
	int nbArrowKeys;
} StructToResolve;

void saveToResolve(StructToResolve * solution)
{
	solution->nbArrowKeys = blendLevel;
	solution->pointer = malloc(solution->nbArrowKeys * sizeof(int));
}

// On va checker toute la grille pour trouver les pièces de taille 1*2 et différencier celle qui le seront verticalement de celle qui le seront horizontalement
void initBack(int grid1[gridSize][gridSize], int grid2[gridSize][gridSize])
{
	int vert = 0, hor = 8;
	for (int line = 0; line < gridSize; ++line)
	{
		for (int col = 0; col < gridSize; ++col) 
		{
			if (grid1[line][col] == grid1[line + 1][col] && line != 3)
			{
				grid2[line][col] = ++vert; 
				grid2[line + 1][col] = vert;
			}

			else if (grid1[line][col] == grid1[line][col + 1] && col != 3)
			{
				grid2[line][col] = ++ hor;
				grid2[line][col + 1] = hor;
			}
		}
	}
}

// Fonction pour afficher les grilles
void printGrid(int grid[gridSize][gridSize])
{
	printf(" 		 --------------------------\n");
	for (int line = 0; line < gridSize; ++line)
	{
		printf("		|");
		for (int col = 0; col < gridSize; ++col)
		{
			if (col == 3)
			{
				if (grid[line][col] == 0) 
				{
					printf("  ");
				}

				else if (grid[line][col] >= 10)
				{
					printf("%d", grid[line][col]);
				}

				else
				{
					//Pour que le dernier élément de la ligne ne fasse pas de tab après lui (pour un affichage ergonomique)
					printf(" %d", grid[line][col]);
				}
			}

			 // Pour que ça nous affiche une "vraie" case vide
			else if (grid[line][col] == 0)
			{
				printf("	");
			}

			else
			{
				printf(" %d 	", grid[line][col]);
			}
		}
		printf(" |");
		printf("\n");
	}
	printf("		 --------------------------\n");
}

// Enregistre la position de notre case vide
void saveEmptyCompartment(int grid[gridSize][gridSize], Compartment *compartment)
{
	for (int line = 0; line < gridSize; ++line)
	{
		for (int col = 0; col < gridSize; ++col) 
		{
			// après avoir parcouru le tableau, lorsqu'on a trouvé la case vide, on enregistre ses nouvelles coordonnées
			if (grid[line][col] == 0)
			{
				compartment->line = line;
				compartment->col = col;
			}
		}
	}
}

// Permet d'afficher les coordonnées d'une case
void printCompartment(Compartment compartment)
{
	printf("{%d, %d}\n", compartment.line,compartment.col);
}

// Permet de comparer 2 grilles
int compareGrid(int grid1[gridSize][gridSize], int grid2[gridSize][gridSize])
{
	// Renvoie 1 si les grilles sont identiques et 0 sinon
	int i = 0;

	for (int line = 0; line < gridSize; ++line)
	{
		for (int col = 0; col < gridSize; ++col)
		{
			if (grid1[line][col] == grid2[line][col])
			{
				i = 1;
			}
			else
			{
				return 0;
			}
		}

	}
	return i;
}

void movementConditions(int grid1[gridSize][gridSize], int grid2[gridSize][gridSize],char inputMovementConditions, Compartment *compartment, int *wrongInput)
{
	saveEmptyCompartment(grid1, compartment);

	// Si l’emplacement vide se trouve en haut de la pièce que tu veux déplacer : appuie sur Z pour la faire monter.
	if (inputMovementConditions == 'Z' && compartment->line != 3)
	{
		// si la pièce d'en bas est de taille 1*1
		if(grid2[compartment->line + 1][compartment->col] == 0)
		{
			grid1[compartment->line][compartment->col] = grid1[compartment->line + 1][compartment->col];
			grid1[compartment->line + 1][compartment->col] = 0;
		}

		// Si la pièce d'en bas est de taille 1*2 verticalement
		else if (grid2[compartment->line + 1][compartment->col] <= 8)
		{
			grid1[compartment->line][compartment->col] = grid1[compartment->line + 2][compartment->col];
			grid1[compartment->line + 2][compartment->col] = 0;
			grid2[compartment->line + 2][compartment->col] = 0;
			grid2[compartment->line][compartment->col] = grid2[compartment->line + 1][compartment->col];
		}

		else
		{
			*wrongInput = inputMovementConditions;
		}
	}

	// Si l’emplacement vide se trouve à gauche de la pièce que tu veux déplacer : appuie sur Q.
	else if (inputMovementConditions == 'Q' && compartment->col != 3)
	{
		// si la pièce de droite est de taille 1*1
		 if(grid2[compartment->line][compartment->col + 1] == 0)
		 {
			grid1[compartment->line][compartment->col] = grid1[compartment->line][compartment->col + 1];
			grid1[compartment->line][compartment->col + 1] = 0;
		}

		// si la pièce de droite est de taille 1*2 horizontalement
		else if (grid2[compartment->line][compartment->col + 1] > 8)
		{
			grid1[compartment->line][compartment->col] = grid1[compartment->line][compartment->col + 2];
			grid1[compartment->line][compartment->col + 2] = 0;
			grid2[compartment->line][compartment->col + 2] = 0;
			grid2[compartment->line][compartment->col] = grid2[compartment->line][compartment->col + 1];
		}

		else
		{
			*wrongInput = inputMovementConditions;
		}
	}

	// Si l’emplacement vide se trouve en bas de la pièce que tu veux déplacer : appuie sur S pour la faire descendre.
	else if (inputMovementConditions == 'S' && compartment->line != 0)
	{
		// si la pièce d'en haut est de taille 1*1
		if(grid2[compartment->line - 1][compartment->col] == 0)
		{
			grid1[compartment->line][compartment->col] = grid1[compartment->line - 1][compartment->col];
			grid1[compartment->line - 1][compartment->col] = 0;
		}

		// Si la pièce d'en haut est de taille 1*2 verticalement
		else if (grid2[compartment->line - 1][compartment->col] <= 8)
		{
			grid1[compartment->line][compartment->col] = grid1[compartment->line - 2][compartment->col];
			grid1[compartment->line - 2][compartment->col] = 0;
			grid2[compartment->line - 2][compartment->col] = 0;
			grid2[compartment->line][compartment->col] = grid2[compartment->line - 1][compartment->col];
		}

		else
		{
			*wrongInput = inputMovementConditions;
		}
	}

	// Si l’emplacement vide se trouve à droite de la pièce que tu veux déplacer : appuie sur D.
	else if (inputMovementConditions == 'D' && compartment->col != 0)
	{
		// si la pièce de gauche est de taille 1*1
		if(grid2[compartment->line][compartment->col - 1] == 0)
		{
			grid1[compartment->line][compartment->col] = grid1[compartment->line][compartment->col - 1];
			grid1[compartment->line][compartment->col - 1] = 0; 
		}

		// si la pièce de gauche est de taille 1*2 horizontalement
		else if (grid2[compartment->line][compartment->col - 1] > 8)
		{
			grid1[compartment->line][compartment->col] = grid1[compartment->line][compartment->col - 2];
			grid1[compartment->line][compartment->col - 2] = 0;
			grid2[compartment->line][compartment->col - 2] = 0;
			grid2[compartment->line][compartment->col] = grid2[compartment->line][compartment->col - 1]; 
		}

		else
		{
			*wrongInput = inputMovementConditions;
		}
	}

	else
	{
		*wrongInput = inputMovementConditions;
	}

}

// On va partir de la grille final pour la mélanger et avoir une résolution de grille que l'on pourra proposer au joueur 
void blend(int grid1[gridSize][gridSize],int grid2[gridSize][gridSize], int grid3[gridSize][gridSize], Compartment *compartment, StructToResolve *solution, int *wrongInput)
{
	srand(time(NULL));
	int randKeys = 0, i = 0, error = 0;
	char inputBlend, arrowKeys[4] = {'Z','S','Q','D'};
	saveToResolve(solution);

	// On initialise la grille de l'utilisateur avec la grille finale
	for (int line = 0; line < gridSize; ++line)
	{
		for (int col = 0; col < gridSize; ++col)
		{
			grid1[line][col] = grid3[line][col];
		}
	}

	initBack(grid1,grid2);
	
	// on mélange cette grille
	while(i < solution->nbArrowKeys)
	{
		saveEmptyCompartment(grid1, compartment);
		randKeys = rand() % 4;
		inputBlend = arrowKeys[randKeys];

		// Va vérifier si la touche directionnelle est à la limite de la grille, si elle est l'inverse de ce qui vient d'être entré ou si elle veut s'appliquer sur une pièce de taille 1*2
		// Si toutes return false, on stock l'inverse de la touche puis on bouge un élément de la grille
		if (inputBlend == 90)
		{
			if (compartment->line == 3 || inputBlend == error || grid2[compartment->line + 1][compartment->col] > 8)
			{
				randKeys = rand() % 4;
				inputBlend = arrowKeys[randKeys];
			}

			else
			{
				solution->pointer[solution->nbArrowKeys - 1 - i] = 83;
				error = 83;
				i++;
				movementConditions(grid1,grid2,inputBlend, compartment, wrongInput);
			}
		}

		else if (inputBlend == 83) 
		{
			if (compartment->line == 0 || inputBlend == error || grid2[compartment->line - 1][compartment->col] > 8)
			{
				randKeys = rand() % 4;
				inputBlend = arrowKeys[randKeys];
			}
			else
			{
				solution->pointer[solution->nbArrowKeys - 1 - i] = 90;
				error = 90;
				i++;
				movementConditions(grid1,grid2,inputBlend, compartment, wrongInput);
			}
			
		}

		else if (inputBlend == 81) 
		{
			if (compartment->col == 3 || inputBlend == error || (grid2[compartment->line][compartment->col + 1] > 0 && grid2[compartment->line][compartment->col + 1] <= 8))	
			{
				randKeys = rand() % 4;
				inputBlend = arrowKeys[randKeys];
			}
			else
			{
				solution->pointer[solution->nbArrowKeys - 1 - i] = 68;
				error = 68;
				i++;
				movementConditions(grid1,grid2,inputBlend, compartment, wrongInput);
			}
			
		}

		else if (inputBlend == 68)
		{
			if (compartment->col == 0 || inputBlend == error || (grid2[compartment->line][compartment->col - 1] > 0 && grid2[compartment->line][compartment->col - 1] <= 8))
			{
				randKeys = rand() % 4;
				inputBlend = arrowKeys[randKeys];
			}
			else
			{
				solution->pointer[solution->nbArrowKeys - 1 - i] = 81;
				error = 81;
				i++;
				movementConditions(grid1, grid2,inputBlend, compartment, wrongInput);
			}
		}
	}
}

// Propose une résolution à l'utilisateur
void resolve(int grid1[gridSize][gridSize], int grid2[gridSize][gridSize], Compartment *compartment, StructToResolve *solution, int *wrongInput)
{
	char inputResolve;
	for (int i = 0; i < solution->nbArrowKeys; ++i)
	{
		inputResolve = solution->pointer[i];
		movementConditions(grid1,grid2,inputResolve, compartment, wrongInput);
		printGrid(grid1);
		sleep(1);
	}
}

void movement(int grid1[gridSize][gridSize], int grid2[gridSize][gridSize],int grid3[gridSize][gridSize], Compartment *compartment, StructToResolve *solution, int *wrongInput, int *keepPlaying) 
{
	saveEmptyCompartment(grid1, compartment);
	char inputMovement;

	while (*keepPlaying == 1) 
	{
		printf("\n 		 À toi de jouer : \n");
		scanf("%c", &inputMovement);

		if (inputMovement == 'z' || inputMovement == 'q' || inputMovement == 's' || inputMovement == 'd' || inputMovement == 'e' || inputMovement == 'r')
		{
			inputMovement -= 32;
			*wrongInput -= 32;
		}

		movementConditions(grid1,grid2,inputMovement, compartment, wrongInput);

		// si tu veux partir : appuie sur E.
		if (inputMovement == 'E')
		{
			*keepPlaying = 0;
			printf(" 		 Tu pars déjà ? Dommage mais à bientôt ! \n\n");
		}

		// pour résoudre le tableau : appuie sur R.
		else if (inputMovement == 'R')
		{
			resolve(grid1,grid2, compartment, solution, wrongInput);
		}

		// On va vérifier ce que le joueur a entré :
			//		Si il a entrée ce qui correspond au tableau, on supprime l'élément du tableau de résolution
			//		Si ce n'est pas une touche du tableau de résolution, on l'enregistre devant
		if (inputMovement == solution->pointer[0]) 
		{
			// on supprime l'élément du tableau de résolution
			for (int i = 0; i < solution->nbArrowKeys - 1; ++i)
			{
				solution->pointer[i] = solution->pointer[i+1];
			}
			--(solution->nbArrowKeys);
		}

		else if(inputMovement != solution->pointer[0] && (inputMovement == 'Z' || inputMovement == 'S' || inputMovement == 'Q' || inputMovement == 'D') && inputMovement != *wrongInput)
		{
			int *pointer = malloc((solution->nbArrowKeys + 1) * sizeof(int));
			for (int i = solution->nbArrowKeys; i > 0; --i)
			{
				pointer[i] = solution->pointer[i-1];
			}

			// Enregistre l'inverse au début du tableau
			if (inputMovement == 90)
			{
				pointer[0] = 83;
			}
			else if (inputMovement == 83)
			{
				pointer[0] = 90;
			}

			else if (inputMovement == 81)
			{
				pointer[0] = 68;
			}

			else if (inputMovement == 68)
			{
				pointer[0] = 81;
			}
			solution->pointer = pointer;
			++(solution->nbArrowKeys);
		}

		saveEmptyCompartment(grid1, compartment);
		initBack(grid1,grid2);
		printGrid(grid1);

		// condition de fin
		if (compareGrid(grid1,grid3))
		{
			printf("\n		 Et voilà, c'est résolu !\n\n");
			*keepPlaying = 0;
		}

		// il faut vider le buffer pour réutiliser le scanf
		else
		{
	 		while(getchar()!='\n');
	 		*wrongInput = 0;
		}
	}
}

int main()
{
	// La grille de jeu pour l'utilisateur, la grille de fond qui nous servira pour les pièces de taille 1*2, la grille à obtenir et La grille à obtenir et une variable qui stockera les entrées bizarre pour les empêcher d'être stocker dans le tableau 
	int initialGrid[gridSize][gridSize], backGrid[gridSize][gridSize] = {0}, finalGrid[gridSize][gridSize] = {{1,2,3,3},{4,4,5,6},{7,8,9,10},{7,11,12,0}}, wrongInput = 0, keepPlaying = 1;

	// Pour enregistrer notre case vide global
	Compartment currentEmptyCompartment;

	// Le Tableau de résolution
	StructToResolve solution;

	// Le Menu d'accueil

	printf("\n\n		 BIENVENUE SUR QUEL TAQUIN !		\n\n");
	printGrid(finalGrid);
	printf("\n	  Je vais te mélanger cette grille et tu devra la remettre comme elle était.");
	printf("\n	  Les commandes sont les suivantes : \n\n");
	printf("	° Si l’emplacement vide se trouve en haut de la pièce que tu veux déplacer : appuie sur Z pour la faire monter.");
	printf("\n	° Si l’emplacement vide se trouve en bas de la pièce que tu veux déplacer : appuie sur S pour la faire descendre.");
	printf("\n	° Si l’emplacement vide se trouve à gauche de la pièce que tu veux déplacer : appuyerie sur Q.");
	printf("\n	° Si l’emplacement vide se trouve à droite de la pièce que tu veux déplacer : appuie sur D.\n");
	printf("\n	À toi de jouer et si tu veux abandonner, tu peux appuyer sur E pour sortir ou sur R si tu veux voir comment tu aurais pu résoudre le jeu.\n	Bon courage ! \n");

	// On mélange la grille de l'utilisateur puis on le la lui présente
	blend(initialGrid,backGrid,finalGrid, &currentEmptyCompartment, &solution, &wrongInput);

	printGrid(initialGrid);

	movement(initialGrid,backGrid,finalGrid, &currentEmptyCompartment, &solution, &wrongInput, &keepPlaying);

	return 0;
}