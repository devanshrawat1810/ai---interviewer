# AI Interviewer: Comprehensive UML Diagrams

This document contains Mermaid diagrams illustrating system models, execution flows, and persistence schemas.

---

## 1. Domain Layer Class Hierarchy

```mermaid
classDiagram
    direction TB

    class Question {
        <<abstract>>
        #string id_
        #string subject_
        #string topic_
        #Difficulty difficulty_
        #string text_
        #string explanation_
        #bool isActive_
        +display()* void
        +evaluate(string)* bool
        +getId() string
        +getDifficulty() Difficulty
    }

    class MCQQuestion {
        -vector~string~ options_
        -string correctAnswer_
        +display() void
        +evaluate(string) bool
    }

    class TextQuestion {
        -string sampleAnswer_
        -vector~string~ keywords_
        +display() void
        +evaluate(string) bool
    }

    class CodingQuestion {
        -string starterCode_
        -vector~TestCase~ testCases_
        +display() void
        +evaluate(string) bool
        +runSolution(string, int) ExecutionResult
    }

    Question <|-- MCQQuestion
    Question <|-- TextQuestion
    Question <|-- CodingQuestion

    class User {
        #string id_
        #string name_
        #string email_
        #string passwordHash_
        #string salt_
        #string role_
        +verifyPassword(string) bool
    }

    class Student {
        -string course_
        -string college_
        -string branch_
        -int streak_
        -int longestStreak_
        -int totalScore_
        -vector~string~ badges_
        +recordActivity() void
    }

    class Admin {
        +canManageQuestions() bool
    }

    User <|-- Student
    User <|-- Admin
```

---

## 2. Service & Repository Layer Architecture (DIP)

```mermaid
classDiagram
    direction LR

    class InterviewSession {
        -string candidateId_
        -vector~shared_ptr~Question~~ questions_
        -shared_ptr~QuestionSelectionStrategy~ strategy_
        +start() void
        +submitAnswer(shared_ptr~Question~, string, int) bool
        +finish() void
    }

    class IQuestionRepository {
        <<interface>>
        +findById(string)* shared_ptr~Question~
        +findBySubject(string)* vector~shared_ptr~Question~~
    }

    class JsonQuestionRepository {
        +findById(string) shared_ptr~Question~
    }

    class MongoQuestionRepository {
        +findById(string) shared_ptr~Question~
    }

    IQuestionRepository <|.. JsonQuestionRepository : Implements
    IQuestionRepository <|.. MongoQuestionRepository : Implements
    InterviewSession ..> IQuestionRepository : Reads questions
```

---

## 3. AI Feedback Sequence with Graceful Fallback

```mermaid
sequenceDiagram
    autonumber
    actor Candidate
    participant InterviewSession
    participant AIFeedback
    participant ILLMClient
    participant RuleBasedFeedback

    Candidate->>InterviewSession: Finishes 10-Question Session
    InterviewSession->>AIFeedback: generateFeedback(session)
    AIFeedback->>ILLMClient: complete(LLMRequest)
    alt API Responds Successfully
        ILLMClient-->>AIFeedback: JSON Response
        AIFeedback->>AIFeedback: Parse qualitative fields
        AIFeedback-->>Candidate: Report with "AI-generated feedback" label
    else API Timeout / Rate Limit / Parse Failure
        ILLMClient--xAIFeedback: AiException Thrown
        AIFeedback->>RuleBasedFeedback: generateFeedback(session)
        RuleBasedFeedback-->>AIFeedback: Deterministic Report
        AIFeedback-->>Candidate: Report with "Offline rule-based fallback" label
    end
```

---

## 4. Database Collections Entity-Relationship (ER) Diagram

```mermaid
erDiagram
    USERS ||--o{ ATTEMPTS : makes
    USERS ||--o{ INTERVIEWS : completes
    QUESTIONS ||--o{ ATTEMPTS : evaluated_in
    INTERVIEWS ||--|| FEEDBACK : generates

    USERS {
        string _id PK
        string name
        string email
        string passwordHash
        string salt
        string role
        string course
        string college
        string branch
        int streak
        int longestStreak
        int totalScore
    }

    QUESTIONS {
        string _id PK
        string subject
        string topic
        string difficulty
        string type
        string text
        string explanation
        bool isActive
    }

    ATTEMPTS {
        string _id PK
        string userId FK
        string questionId FK
        string answer
        bool isCorrect
        int score
        int elapsedSeconds
        string date
    }

    INTERVIEWS {
        string _id PK
        string userId FK
        string course
        int totalScore
        int maxScore
        double percentage
        string completedAt
    }

    FEEDBACK {
        string _id PK
        string interviewId FK
        string overallSummary
        int communicationClarity
        int answerStructure
        int confidenceCues
        bool isAiGenerated
    }
```
