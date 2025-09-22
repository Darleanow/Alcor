# Partie I – Fondations

### Introduction au développement kernel

---

Bonjour et bienvenue développeurs !
Dans ce cours, nous nous efforcerons de vous apprendre les bases du développement Kernel, les différentes étapes à suivre.
Pour suivre ce cours convenablement, vous aurez besoin d'être familier avec le développement en général, de solides bases en langage C, ainsi que quelques connaissances en Assembleur (nous utiliserons NASM, mais libre à vous de prendre une autre version si vous vous sentez à l'aise).

Concernant nos profils, nous sommes 3 à avoir construit ce cours :

* Damien Nithard : Développeur/Architecte logiciel
* Jory Grezczszak : Développeur embarqué
* Enzo Hugonnier : Développeur/Architecte logiciel

Nous interviendrons chacun de notre côté au fur et à mesure de ce cours.
Le résultat final sera accessible via github à but de correction.

##### Différence kernel / userland

Avant de commencer à coder, nous passerons par une légère partie théorique pour comprendre les bases, nous vous conseillons de ne pas passer cette partie car elle vous sera utile tout au long de votre apprentissage.

Dans un processeur (CPU), il existe différents anneaux de privilèges, un peu comme des droits administrateurs.
Le ring 0 (aussi appelé kernel land) est le mode administrateur absolu au sein d'un CPU : tout est permis, ce qui peut facilement provoquer des plantages si on n'est pas prudent. On y retrouve tout le code qui doit accéder au hardware directement.

Les ring 1 et 2, souvent réservés aux drivers, mais rarement utilisés dans des architectures monolithiques (plus souvent dans des microkernels ou systèmes expérimentaux), nous ignorerons donc ces deux anneaux pour nous concentrer sur le 0 ainsi que le 3.

Le ring 3 (aussi appelé user land) est l'anneau applicatif, l'utilisateur ne peut pas exécuter des instructions sensibles (écriture mémoire ou manipulation hardware).

NB : Ici nous parlons de mémoire physique, contrairement à la mémoire virtuelle qui est accessible par l'utilisateur, la différence sera détaillée ultérieurement.

##### Pourquoi monolithique ?

L'architecture monolithique est historique, inventée en 1960.
Une des raisons de pourquoi nous l'avons choisie vient de son accessibilité : ici, nous coderons tout au même endroit.

Il existe de nouvelles variantes, plus sûres, comme l'architecture micronoyau, où l'idée est de minimiser le plus possible l'applicatif situé dans le kernel.
Cette approche est plus sûre, mais aussi plus complexe, ce pourquoi nous nous orientons vers un système plus classique.

##### Architecture x86/x86_64

--> TODO

##### Outils nécessaires (cross-compiler, QEMU, GDB)

Pour suivre ce cours de la manière la plus adaptée possible, il vous faudra installer plusieurs utilitaires :

* Qemu (Virtualisation) : `sudo apt install qemu-system-x86_64`
* Nasm (NetWide Assembly) : `sudo apt install nasm`
* GDB (Debuggeur) : `sudo apt install gdb`

Concernant le cross-compiler, nous détaillerons cette étape un peu plus tard.

### Bootloader et démarrage de la machine

Le rôle du bootloader au sein d'un OS est de faire la passerelle entre le Kernel et le BIOS de la machine, autrement dit les composants électroniques si nous simplifions.
Lorsqu'une machine démarre, le BIOS (ou l'UEFI en mode compatibilité) exécute en premier les instructions contenues dans le premier secteur du disque, appelé Master Boot Record (MBR).
Ce secteur ne fait que 512 octets et doit se terminer par la signature 0x55AA pour être reconnu.
Le bootloader doit donc être extrêmement compact et écrit en assembleur pour respecter cette contrainte.

Ses tâches principales sont :

* Initialiser le minimum nécessaire du processeur et de la mémoire pour pouvoir charger le noyau.
* Localiser l'image du noyau sur le support de stockage.
* Charger cette image en mémoire à une adresse prédéfinie.
* Transférer l'exécution au noyau en sautant à l'adresse d'entrée de ce dernier.

Une fois ce transfert effectué, le rôle du bootloader est terminé.
Le kernel prend alors le contrôle total de la machine et commence sa propre séquence d'initialisation.

### Explication pas à pas du MBR (Master Boot Record)

Le BIOS charge automatiquement les 512 octets du premier secteur du disque à l'adresse physique 0x7C00 et démarre l'exécution à cet endroit. Le code suivant illustre un MBR minimal qui affiche un message puis arrête le CPU. Voici ce que chaque instruction fait concrètement.

```asm
BITS 16
ORG 0x7C00
```

On informe l'assembleur que l'on code en 16 bits, car le BIOS démarre en mode réel 16 bits. On précise que le code sera exécuté à l'adresse 0x7C00, l'endroit exact où le BIOS place le MBR.

```asm
cli
xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7C00
sti
```

On désactive les interruptions (`cli`) le temps de configurer les segments. On met AX à zéro, puis on initialise DS, ES et SS à 0 pour que les segments pointent tous au début de la mémoire physique. On place la pile juste sous le secteur chargé (SP = 0x7C00). Ensuite on réactive les interruptions (`sti`).

```asm
mov si, msg1
```

On met dans SI l'adresse du message à afficher. SI servira de pointeur pour lire le texte.

```asm
print_char:
    lodsb
    or  al, al
    jz  hang
    mov ah, 0x0E
    mov bh, 0
    mov bl, 0x07
    int 0x10
    jmp print_char
```

`lodsb` charge dans AL l'octet pointé par SI puis SI avance. `or al, al` teste si AL vaut 0 (fin de chaîne). Si oui, on saute à `hang`. Sinon on prépare un appel BIOS vidéo : AH=0x0E indique le mode teletype, BH=0 page 0, BL=0x07 couleur gris clair sur noir. `int 0x10` envoie le caractère contenu dans AL à l'écran. On boucle jusqu'à la fin du message.

```asm
hang:
    hlt
    jmp hang
```

On arrête le CPU (`hlt`) et on boucle pour rester figé. Sans boucle, un réveil d'interruption pourrait continuer l'exécution au hasard.

```asm
msg1 db "Hello, world!", 0x0D, 0x0A, 0
```

Chaîne terminée par un octet nul, suivie d'un retour chariot (0x0D) et d'un saut de ligne (0x0A).

```asm
times 510-($-$$) db 0
dw 0xAA55
```

On remplit de zéros jusqu'au 510e octet. Puis on écrit la signature 0xAA55. C'est ce marquage qui permet au BIOS de reconnaître ce secteur comme amorçable.

Ce programme illustre la fonction minimale d'un MBR : être chargé par le BIOS, exécuter un code minuscule directement en mode réel, afficher un message, puis arrêter la machine.

Voici le code complet du MBR que nous venons de créer:

```asm 
; Master Boot Record (MBR)
; Prints a short message then halts the CPU

BITS 16                 ; BIOS runs the boot sector in 16-bit real mode
ORG 0x7C00              ; BIOS loads the sector at physical address 0x7C00

start:
    ; Set up segments and stack
    cli                 ; Disable interrupts while configuring segments
    xor ax, ax          ; AX = 0
    mov ds, ax          ; Data segment = 0
    mov es, ax          ; Extra segment = 0
    mov ss, ax          ; Stack segment = 0
    mov sp, 0x7C00      ; Place stack just below the boot sector
    sti                 ; Re-enable interrupts

    mov si, msg1        ; SI points to the message string

print_char:
    lodsb               ; Load next byte from [SI] into AL, SI++
    or  al, al          ; Test for end of string (0 byte)
    jz  hang            ; If zero, jump to halt loop
    mov ah, 0x0E        ; BIOS teletype function
    mov bh, 0           ; Display page 0
    mov bl, 0x07        ; Text attribute: light gray on black
    int 0x10            ; Call BIOS video service
    jmp print_char      ; Continue with next character

hang:
    hlt                 ; Halt the CPU until an interrupt occurs
    jmp hang            ; Stay here forever

msg1 db "Hello, world!", 0x0D, 0x0A, 0  ; Message followed by CR LF and terminator

times 510-($-$$) db 0   ; Pad with zeros up to byte 510
dw 0xAA55               ; Boot signature required by BIOS
```

Nous vous recommandons de créer un dossier du nom de votre kernel, d'y ajouter un meme dossier `boot/`, et de sauvegarder le code fourni précédemment en tant que fichier `mbr.asm`.

Pour tester ce code, rien de plus simple, il vous suffira d'entrer ces commandes:
```bash
mkdir -p build
nasm -f bin boot/mbr.asm -o build/mbr.bin
qemu-system-x86_64 -drive format=raw,file=build/mbr.bin
```

Vous devriez voir apparaître le message: `Hello, world!` à l'écran, puis constater une boucle infinie causée par `hang`.

Félicitations ! Vous venez d'écrire votre tout premier kernel !

# Partie II - Gestion mémoire

### Segmentation et GDT

### Interruptions et IDT

### Paging et mémoire virtuelle

### Allocateurs mémoire

# Partie III - Intéractions avec le matériel

### Programmation et interruptions matérielles (PIC/APIC)

### Timer et multitâches basique

### Drivers de périphériques simples

# Partie IV - Processus et Syscalls

### Structure de processus et scheduling

### Appels système (syscalls)

### Chargement et exécution des programmes 

# Partie V - Userland et système minimal

### Le processus init

### Ecriture d'un shell minimal

### Gestion de fichiers simplifiée

### Extension des syscalls

# Partie VI - Conclusion et perspectives

### Etat final du système

### Optimisations possibles

### Ouvertures

