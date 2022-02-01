package com.example.first_project;

import java.util.List;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;
@Repository
public interface ExamRepository extends JpaRepository<Exam, Long>{
    List<Exam> findByName(String name);
}