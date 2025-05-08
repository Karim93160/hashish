# Guide des Contributions à Hashish

Nous vous remercions de votre intérêt à contribuer au projet Hashish ! Votre aide est précieuse pour améliorer cet outil open source. Ce guide a pour objectif de vous aider à contribuer de manière efficace et respectueuse.

## Comment Contribuer

Il existe plusieurs façons de contribuer à Hashish :

* **Signaler des problèmes (Bugs et Demandes de Fonctionnalités) :** Si vous rencontrez un bug, une erreur ou si vous avez une idée pour une nouvelle fonctionnalité, veuillez ouvrir un [nouveau problème (issue)](https://github.com/Karim93160/hashish/issues/new) sur GitHub. Soyez aussi précis que possible dans votre description, en incluant les étapes pour reproduire le problème ou en expliquant clairement votre proposition de fonctionnalité.

* **Soumettre des Pull Requests (Corrections de Bugs et Nouvelles Fonctionnalités) :** Si vous souhaitez corriger un bug ou implémenter une fonctionnalité vous-même, vous pouvez le faire en soumettant une pull request (PR). Voici les étapes à suivre :

    1.  **Forkez le dépôt :** Cliquez sur le bouton "Fork" en haut à droite de la page du dépôt GitHub. Cela créera une copie du dépôt sur votre propre compte.

    2.  **Clonez votre fork localement :**
        ```bash
        git clone [https://github.com/VotreNomUtilisateur/hashish.git](https://github.com/VotreNomUtilisateur/hashish.git)
        cd hashish
        ```
        (Remplacez `VotreNomUtilisateur` par votre nom d'utilisateur GitHub).

    3.  **Créez une branche pour votre contribution :** Choisissez un nom descriptif pour votre branche (par exemple, `fix-bug-x` ou `feature-y`).
        ```bash
        git checkout -b nom-de-votre-branche
        ```

    4.  **Apportez vos modifications :** Codez, testez et assurez-vous que vos changements fonctionnent correctement. Respectez les conventions de codage du projet (si elles existent).

    5.  **Commitez vos modifications :** Utilisez des messages de commit clairs et concis expliquant ce que vous avez fait.
        ```bash
        git add .
        git commit -m "Correction du bug X : description du correctif"
        ```
        ou
        ```bash
        git commit -m "Ajout de la fonctionnalité Y : description de la fonctionnalité"
        ```

    6.  **Poussez votre branche vers votre fork sur GitHub :**
        ```bash
        git push origin nom-de-votre-branche
        ```

    7.  **Soumettez une Pull Request :** Sur la page de votre fork sur GitHub, cliquez sur le bouton "Compare & pull request". Rédigez une description claire de votre pull request, expliquant les changements que vous proposez et pourquoi.

* **Améliorer la Documentation :** La documentation est essentielle ! Si vous voyez des erreurs, des omissions ou des zones qui pourraient être plus claires, n'hésitez pas à soumettre des PR pour améliorer la documentation (fichiers README, commentaires dans le code, etc.).

* **Traduire Hashish :** Si vous maîtrisez d'autres langues, vous pouvez aider à traduire l'interface utilisateur, la documentation ou d'autres éléments du projet.

* **Tester Hashish :** Utilisez l'outil, signalez les bugs que vous trouvez et aidez à valider les correctifs proposés dans les pull requests.

## Conventions de Codage

[**Ici, vous pouvez ajouter des directives spécifiques sur le style de codage, les conventions de nommage, etc. Si vous n'en avez pas encore, vous pouvez laisser cette section vide pour l'instant ou indiquer que vous suivez les conventions Python standard (PEP 8) pour le code Python.**]

Par exemple :

> Pour le code Python, nous suivons généralement les directives de style définies dans la PEP 8. Veuillez vous assurer que votre code est bien formaté et facile à lire.

## Tests

[**Si votre projet a des tests automatisés, expliquez ici comment les exécuter et encouragez les contributeurs à ajouter des tests pour leurs modifications.**]

Par exemple :

> Veuillez vous assurer que vos modifications sont accompagnées de tests unitaires pertinents. Pour exécuter les tests, utilisez la commande `pytest` dans le répertoire racine du projet.

## Processus de Révision des Pull Requests

* Les pull requests seront examinées par les mainteneurs du projet.
* Nous pouvons demander des modifications avant de fusionner une PR.
* Soyez patient et réactif à la discussion et aux demandes de modifications.
* Une fois approuvée, votre PR sera fusionnée dans la branche principale.

## Code de Conduite

Nous attendons de tous les contributeurs qu'ils adhèrent à un [Code de Conduite](https://www.contributor-covenant.org/version/2/0/code_of_conduct.html) (lien vers un Code de Conduite standard, comme le Contributor Covenant, que vous pouvez ajouter à votre dépôt). Veuillez traiter les autres contributeurs avec respect et professionnalisme.

## Questions ?

Si vous avez des questions sur la contribution, n'hésitez pas à ouvrir un [nouveau problème (issue)](https://github.com/Karim93160/hashish/issues/new) ou à contacter [votre adresse e-mail de contact].

Merci encore pour votre contribution !

---

*Ce guide est basé sur des pratiques courantes dans les projets open source. N'hésitez pas à l'adapter aux spécificités de votre projet Hashish.*
